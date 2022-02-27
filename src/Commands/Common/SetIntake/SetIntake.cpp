// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SetIntake.h"

SetIntake::SetIntake(Intake* intake, double voltage, bool pistonSet) {
  // Use addRequirements() here to declare subsystem dependencies.
  this->intake = intake;
  this->voltage = voltage;
  this->pistonSet = pistonSet;
  AddRequirements(intake);
}

// Called when the command is initially scheduled.
void SetIntake::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void SetIntake::Execute() {
  intake->setPistons(pistonSet);
  intake->setVoltage(voltage);
}

// Called once the command ends or is interrupted.
void SetIntake::End(bool interrupted) {}

// Returns true when the command should end.
bool SetIntake::IsFinished() { return true; }
