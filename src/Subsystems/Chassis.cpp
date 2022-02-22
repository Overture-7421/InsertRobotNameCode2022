/*
    ____                     __     ____        __          __     _   __                   
   /  _/___  ________  _____/ /_   / __ \____  / /_  ____  / /_   / | / /___ _____ ___  ___ 
   / // __ \/ ___/ _ \/ ___/ __/  / /_/ / __ \/ __ \/ __ \/ __/  /  |/ / __ `/ __ `__ \/ _ \
 _/ // / / (__  )  __/ /  / /_   / _, _/ /_/ / /_/ / /_/ / /_   / /|  / /_/ / / / / / /  __/
/___/_/ /_/____/\___/_/   \__/  /_/ |_|\____/_.___/\____/\__/  /_/ |_/\__,_/_/ /_/ /_/\___/                                               
*/

#include "Chassis.h"

#include <iostream>
#include <frc/trajectory/constraint/CentripetalAccelerationConstraint.h>

Chassis::Chassis() {
  ahrs.Calibrate();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  double startTime = frc::Timer::GetFPGATimestamp().value();
  while (ahrs.IsCalibrating()) {
    double timePassed = frc::Timer::GetFPGATimestamp().value() - startTime;
    if (timePassed > 10) {
      std::cout << "ERROR!!!!: NavX took too long to calibrate." << std::endl;
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  ahrs.ZeroYaw();

  rightSlave1.Follow(rightMaster);
  rightSlave2.Follow(rightMaster);

  rightMaster.SetInverted(InvertType::InvertMotorOutput);
  rightSlave1.SetInverted(InvertType::InvertMotorOutput);
  rightSlave2.SetInverted(InvertType::InvertMotorOutput);
  
  leftSlave1.Follow(leftMaster);
  leftSlave2.Follow(leftMaster);

  rightMaster.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor);
  leftMaster.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor);

  rightMaster.SetSelectedSensorPosition(0.0);
  leftMaster.SetSelectedSensorPosition(0.0);

  rightMaster.SetNeutralMode(NeutralMode::Brake);
  leftMaster.SetNeutralMode(NeutralMode::Brake);

  rightMaster.ConfigSupplyCurrentLimit(SupplyCurrentLimitConfiguration(true, 30, 0, 1));
  leftMaster.ConfigSupplyCurrentLimit(SupplyCurrentLimitConfiguration(true, 30, 0, 1));

  frc::SmartDashboard::PutData("RobotPose", &field);
  frc2::CommandScheduler::GetInstance().RegisterSubsystem(this);
}

const frc::Pose2d& Chassis::getPose() {
    pe_NMutex.lock();
    std::lock_guard<std::mutex> lg(pe_MMutex);
    pe_NMutex.unlock();
    return odometry.GetEstimatedPosition(); 
}

frc2::SequentialCommandGroup Chassis::getRamseteCommand(
    const std::vector<frc::Pose2d>& waypoints, frc::TrajectoryConfig config,
    bool reversed) {
  config.SetReversed(reversed);

  config.AddConstraint(frc::CentripetalAccelerationConstraint(units::meters_per_second_squared_t(5)));

  auto targetTrajectory =
      frc::TrajectoryGenerator::GenerateTrajectory(waypoints, config);
  auto ramseteController = frc::RamseteController();
  auto commandGroup = frc2::SequentialCommandGroup();

  commandGroup.AddCommands(
      frc2::RamseteCommand(
          targetTrajectory, [this]() { return getPose(); }, ramseteController,
          kinematics,
          [this](units::meters_per_second_t leftVel,
                 units::meters_per_second_t rightVel) {
            frc::DifferentialDriveWheelSpeeds wheelSpeeds;
            wheelSpeeds.left = leftVel;
            wheelSpeeds.right = rightVel;

            setVelocities(kinematics.ToChassisSpeeds(wheelSpeeds));
          },
          {this}),
      frc2::InstantCommand([this]() { setVelocities({}); }, {this}));

  return commandGroup;
}

void Chassis::setVelocities(frc::ChassisSpeeds vels) {
  frc::DifferentialDriveWheelSpeeds wheelVels = kinematics.ToWheelSpeeds(vels);

  wheelVels.Desaturate(units::meters_per_second_t(getMaxVelocity()));
  
  leftTargetVel = wheelVels.left.value();
  rightTargetVel = wheelVels.right.value();
}

void Chassis::resetOdometry(frc::Pose2d pose) {
  pe_NMutex.lock();
  pe_MMutex.lock();
  pe_NMutex.unlock();
  odometry.ResetPosition(pose, units::degree_t(-ahrs.GetYaw()));
  pe_MMutex.unlock();

  leftMaster.SetSelectedSensorPosition(0.0);
  rightMaster.SetSelectedSensorPosition(0.0);
}

double Chassis::getMaxVelocity(){
  return maxSpeed;
}

void Chassis::addVisionMeasurement(const frc::Pose2d& visionPose, double timeStamp){
      //Low priority lock
    pe_LMutex.lock();
    pe_NMutex.lock();
    pe_MMutex.lock();
    pe_NMutex.unlock();
    odometry.AddVisionMeasurement(visionPose, units::second_t(timeStamp));
    pe_MMutex.unlock();
    pe_LMutex.unlock();
}

// This method will be called once per scheduler run
void Chassis::Periodic() {
  updatePIDs();
  updateTelemetry();
  rightDistance = convertToMeters(rightMaster.GetSelectedSensorPosition());
  leftDistance = convertToMeters(leftMaster.GetSelectedSensorPosition());

  rightVel = convertToMetersPerSec(rightMaster.GetSelectedSensorVelocity());
  leftVel = convertToMetersPerSec(leftMaster.GetSelectedSensorVelocity());

  frc::Rotation2d gyroAngle{units::degree_t(-ahrs.GetYaw())};

  frc::DifferentialDriveWheelSpeeds wheelSpeeds;
  wheelSpeeds.left = units::meters_per_second_t(leftVel);
  wheelSpeeds.right = units::meters_per_second_t(rightVel);
  pe_NMutex.lock();
  pe_MMutex.lock();
  pe_NMutex.unlock();
  odometry.Update(gyroAngle, wheelSpeeds, units::meter_t(leftDistance),
                                units::meter_t(rightDistance));
  pe_MMutex.unlock();
}

void Chassis::updatePIDs() {
  const auto leftVelTargetMps = units::meters_per_second_t(leftTargetVel);
  const auto rightVelTargetMps = units::meters_per_second_t(rightTargetVel);

  const auto leftLimitedTarget = leftAccelLimiter.Calculate(leftVelTargetMps);
  const auto rightLimitedTarget = rightAccelLimiter.Calculate(rightVelTargetMps);

  leftPID.SetSetpoint(leftLimitedTarget.value());
  rightPID.SetSetpoint(rightLimitedTarget.value());

  const auto leftOutput = units::volt_t(leftPID.Calculate(leftVel)) +
                          ff.Calculate(leftLimitedTarget);
  const auto rightOutput = units::volt_t(rightPID.Calculate(rightVel)) +
                           ff.Calculate(rightLimitedTarget);

  leftMaster.SetVoltage(leftOutput);
  rightMaster.SetVoltage(rightOutput);
}

void Chassis::updateTelemetry() {
  frc::SmartDashboard::PutNumber("Chassis/RightSensorRawPos",
                                 rightMaster.GetSelectedSensorPosition());
  frc::SmartDashboard::PutNumber("Chassis/LeftSensorRawPos",
                                 leftMaster.GetSelectedSensorPosition());

  frc::SmartDashboard::PutNumber("Chassis/RightSensorMetersPos", rightDistance);
  frc::SmartDashboard::PutNumber("Chassis/LeftSensorMetersPos", leftDistance);

  frc::SmartDashboard::PutNumber("Chassis/RightVelocity", rightVel);
  frc::SmartDashboard::PutNumber("Chassis/LeftVelocity", leftVel);

  field.SetRobotPose(getPose());
  
}

double Chassis::convertToMeters(double sensorRawPos) {
  double sensorMeterPos =
      sensorRawPos / encoder_CodesPerRev * 2 * M_PI * wheelRadius;

  return sensorMeterPos;
}

double Chassis::convertToMetersPerSec(double rawEncoderVel100ms) {
  double rawEncoderVel = rawEncoderVel100ms * 10;
  double revPerSec = rawEncoderVel / encoder_CodesPerRev;
  double angularVel = revPerSec * 2 * M_PI;
  double metersPerSec = angularVel * wheelRadius;

  return metersPerSec;
}
