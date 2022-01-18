// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AlignToTower.h"

AlignToTower::AlignToTower(Chassis* chassis) {
  // Use addRequirements() here to declare subsystem dependencies.
  this->chassis = chassis;
  AddRequirements(chassis);
}

// Called when the command is initially scheduled.
void AlignToTower::Initialize() {
  
}

// Called repeatedly when this Command is scheduled to run
void AlignToTower::Execute() {
}

// Called once the command ends or is interrupted.
void AlignToTower::End(bool interrupted) {}

// Returns true when the command should end.
bool AlignToTower::IsFinished() {
  return false;
}
