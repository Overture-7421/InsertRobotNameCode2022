// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/SubsystemBase.h>
#include <frc/geometry/Transform2d.h>
#include <frc/geometry/Pose2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <photonlib/PhotonCamera.h>
#include <ntcore_cpp.h>
#include "Eigen/Core"

#include "Subsystems/SwerveChassis/SwerveChassis.h"

class VisionManager : public frc2::SubsystemBase {
public:
  VisionManager(SwerveChassis *chassis);

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  frc::Rotation2d QuaternionToEuler(std::vector<double> quaternion);

private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.
  photonlib::PhotonCamera camera{ "overcamara" };
  frc::Transform2d fieldToTarget{ {2.0_m, -0.5_m}, {90_deg} };
  frc::Transform2d cameraToRobot{ {0_m,0_m},{0_deg} };

  SwerveChassis *chassis;

  nt::NetworkTableInstance ntInstance = nt::NetworkTableInstance::GetDefault();
  nt::NetworkTableEntry visionPoseEntry = ntInstance.GetEntry("/photonvision/overcamara/targetPose");

};
