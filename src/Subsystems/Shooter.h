// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <ctre/Phoenix.h>
#include <frc2/command/SubsystemBase.h>
#include <units/angular_velocity.h>
#include <frc/system/plant/LinearSystemId.h>
#include <frc/estimator/KalmanFilter.h>
#include <frc/system/LinearSystemLoop.h>
#include <frc/controller/LinearQuadraticRegulator.h>
#include <frc/filter/SlewRateLimiter.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>

using namespace ctre::phoenix::motorcontrol::can;
class Shooter : public frc2::SubsystemBase {
 public:
  Shooter();
  void setVelocity(double radsPerS);
  bool reachedVelocityTarget();
  double getVelocity();
  void Periodic() override;

 private:
  WPI_TalonFX rightShooter{4};
  WPI_TalonFX leftShooter{5};

  int encoder_CodesPerRev = 2048;


  double radsPerSecond = 0.0;
  double tolerance = 5;
  const double timeToStableRPS = 0.2;  // Seconds
  double lastTimeStable = 0;
  bool lastOnTargetState = false;
  bool stabilizedOnTarget = false;
  
  // TODO Caracterizar disparador
  frc::SlewRateLimiter<units::radian> limiter {150_rad_per_s};
  
  // Volts 
  static constexpr auto kFlywheelKs = 0.55611_V; 

  // Volts per (radian per second)
  static constexpr auto kFlywheelKv = 0.017193_V / 1_rad_per_s; 

  // Volts per (radian per second squared)
  static constexpr auto kFlywheelKa = 0.0020744_V / 1_rad_per_s_sq;

   frc2::PIDController shooterController {0.022523, 0, 0};
frc::SimpleMotorFeedforward<units::radian> shooterFF {kFlywheelKs, kFlywheelKv, kFlywheelKa};
};
