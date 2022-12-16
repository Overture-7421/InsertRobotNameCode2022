// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "FollowSwervePath.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
FollowSwervePath::FollowSwervePath(SwerveChassis* swerveChassis, std::vector<frc::Pose2d> positions, frc::TrajectoryConfig trajectoryConfig) : swerveChassis(swerveChassis) {
  // Add your commands here, e.g.
  // AddCommands(FooCommand(), BarCommand());
  auto swerveKinematics = swerveChassis->getKinematics();
  trajectoryConfig.SetKinematics(swerveKinematics);

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
    positions,
    trajectoryConfig
  );

  frc::ProfiledPIDController<units::radian> thetaController(
    5, 0, 0, { wpi::numbers::pi * 4_rad_per_s, wpi::numbers::pi * 8_rad_per_s / 1_s }
  );

  thetaController.EnableContinuousInput(-180_deg, 180_deg);

  frc2::SwerveControllerCommand<4> controllerCommand(
    trajectory,
    [this]() {return this->swerveChassis->getOdometry();},
    swerveChassis->getKinematics(),
    frc::PIDController(5, 0, 0),
    frc::PIDController(5, 0, 0),
    thetaController,
    [this](auto desiredStates) {this->swerveChassis->setModuleStates(desiredStates);},
    { swerveChassis }
  );

  frc2::InstantCommand stopCommand([this]() {this->swerveChassis->setSpeed(0, 0, 0);});

  AddCommands(controllerCommand, stopCommand);
}
