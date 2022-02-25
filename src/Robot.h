/*
    ____                     __     ____        __          __     _   __
   /  _/___  ________  _____/ /_   / __ \____  / /_  ____  / /_   / | / /___
_____ ___  ___
   / // __ \/ ___/ _ \/ ___/ __/  / /_/ / __ \/ __ \/ __ \/ __/  /  |/ / __ `/
__ `__ \/ _ \
 _/ // / / (__  )  __/ /  / /_   / _, _/ /_/ / /_/ / /_/ / /_   / /|  / /_/ / /
/ / / /  __/
/___/_/ /_/____/\___/_/   \__/  /_/ |_|\____/_.___/\____/\__/  /_/ |_/\__,_/_/
/_/ /_/\___/
*/

#pragma once

#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc2/command/Command.h>

#include "Subsystems/Chassis/Chassis.h"
#include "Subsystems/Shooter/Shooter.h"
#include "Subsystems/VisionManager/VisionManager.h"
#include "Subsystems/RangeDecider/RangeDecider.h"

#include "Commands/Autonomous/RamseteTests/RamseteTests.h"
#include "Commands/Autonomous/TurnToAngle/TurnToAngle.h"
#include "Commands/Autonomous/CommandTest/CommandTest.h"
#include "Commands/Teleop/DefaultDrive/DefaultDrive.h"


class Robot : public frc::TimedRobot {
 public:
  void RobotInit() override;
  void RobotPeriodic() override;
  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void TeleopInit() override;
  void TeleopPeriodic() override;
  void DisabledInit() override;
  void DisabledPeriodic() override;
  void TestInit() override;
  void TestPeriodic() override;

 private:
  std::unique_ptr<frc2::SequentialCommandGroup> autocommand;
  frc::Joystick joy {0};

  //Subsystems
  Chassis chassis;
  VisionManager visionManager {&chassis};
  Shooter shooter;
  RangeDecider rangeDecider;

  //Default Commands
  DefaultDrive defaultDrive {&chassis, &joy};

};
