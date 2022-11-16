// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/SubsystemBase.h>
#include <ctre/Phoenix.h>
#include "frc/DataLogManager.h"
#include "frc/DriverStation.h"
#include <frc/Joystick.h>



class Shooter : public frc2::SubsystemBase {
 public:
  Shooter();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Shoot(double x) {

    shooter_Left.Set(-x);
    shooter_Right.Set(x);
  
  }

  void Calc_Vel (double vel) {
 shooter_Left.read(vel);
 
  }
  void Periodic() override;

 private:
  WPI_TalonFX shooter_Left{14};
  WPI_TalonFX shooter_Right{15};

  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.
  //metodo double para vel de llanta
};
