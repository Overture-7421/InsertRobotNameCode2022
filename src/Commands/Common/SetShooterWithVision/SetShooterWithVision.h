// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

#include "Subsystems/Shooter/Shooter.h"
#include "Subsystems/VisionManager/VisionManager.h"
#include "Utils/Interpolation/LinearInterpolator/LinearInterpolator.h"

/**
 * An example command.
 *
 * <p>Note that this extends CommandHelper, rather extending CommandBase
 * directly; this is crucially important, or else the decorator functions in
 * Command will *not* work!
 */
class SetShooterWithVision
    : public frc2::CommandHelper<frc2::CommandBase, SetShooterWithVision> {
 public:
  SetShooterWithVision(Shooter* shooter, VisionManager* visionManager);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  Shooter* shooter;
  VisionManager* visionManager;
  LinearInterpolator distanceVsVelocityInterpolator {{
    {1.8, 345},
    {2.3, 366},
    {2.8, 385},
    {3.3, 410},
    {3.8, 427},
    {4.3, 452},
    {4.8, 468},
    {5.3, 488}
  }};
};
