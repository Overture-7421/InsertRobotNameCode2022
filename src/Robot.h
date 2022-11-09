#pragma once
#include <frc/TimedRobot.h>
#include <Subsystems/Intake/Intake.h>
#include "Subsystems/Climber/Climber.h"
#include <frc/Joystick.h>

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
  Intake intake;
  Climber climber;
  frc::Joystick joystick{0};
  
};
