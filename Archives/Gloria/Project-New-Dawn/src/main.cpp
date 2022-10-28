/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Ellie Britt                                               */
/*    Created:      Sep 1, 2021                                               */
/*    Description:  Main File                                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// RFM                  motor         20              
// LFM                  motor         10              
// RBM                  motor         11              
// LBM                  motor         1               
// InertSens            inertial      16              
// DR4B_left            motor         2               
// DR4B_right           motor         3               
// Intake               motor         9               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include <vex.h>
#include "Auton.h"

competition Competition;
Auton au;

float xComp, yComp, rotComp;  // Controller Variables

void pre_auton(void)
{
  vexcodeInit();
  task posSens(au.position_scan());
  DR4B_right.setStopping(vex::hold);
  DR4B_left.setStopping(vex::hold);
  InertSens.calibrate();
}

void autonomous(void)
{
  au.auton_select();
}

void usercontrol(void)
{
  InertSens.calibrate();
  DR4B_right.spin(vex::forward);
  DR4B_left.spin(vex::forward);
  Intake.spin(vex::forward);
  
  while (true)
  {
    xComp = Controller1.Axis4.position()*(abs(Controller1.Axis4.position())>=5);
    yComp = Controller1.Axis3.position()*(abs(Controller1.Axis3.position())>=5);
    rotComp = Controller1.Axis1.position()*(abs(Controller1.Axis1.position())>=5);

    if (xComp < 0)  // Grab the positivity of the X-input and apply curve
    {
      xComp = 1.2*pow(1.043, -xComp) - 1.2 + 0.2*(-xComp);
      xComp = -xComp;
    }
    else
    {
      xComp = 1.2*pow(1.043, xComp) - 1.2 + 0.2*(xComp);
    }
    if (yComp < 0)  // Grab the positivity of Y-input and apply curve
    {
      yComp = 1.2*pow(1.043, -yComp) - 1.2 + 0.2*(-yComp);
      yComp = -yComp;
    }
    else
    {
      yComp = 1.2*pow(1.043, yComp) - 1.2 + 0.2*(yComp);
    }
    if (rotComp < 0)  // Grab the positivity of Rotation and apply curve (Post filter)
    {
      rotComp = 1.2*pow(1.043, -rotComp) - 1.2 + 0.2*(-rotComp);
      rotComp = -rotComp;
    }
    else
    {
      rotComp = 1.2*pow(1.043, rotComp) - 1.2 + 0.2*(rotComp);
    }

    if (Controller1.ButtonR1.pressing())
    {
      DR4B_right.setVelocity(-20, vex::percent);
      DR4B_left.setVelocity(-20, vex::percent);
      DR4B_right.spin(vex::forward);
      DR4B_left.spin(vex::forward);
    }
    else if (Controller1.ButtonR2.pressing())
    {
      DR4B_right.setVelocity(20, vex::percent);
      DR4B_left.setVelocity(20, vex::percent);
      DR4B_right.spin(vex::forward);
      DR4B_left.spin(vex::forward);
    }
    else
    {
      DR4B_right.setVelocity(0, vex::percent);
      DR4B_left.setVelocity(0, vex::percent);
      DR4B_left.stop(vex::hold);
      DR4B_right.stop(vex::hold);
    }

    if (Controller1.ButtonL1.pressing())
    {
      Intake.setVelocity(-75, vex::percent);
    }
    else if (Controller1.ButtonL2.pressing())
    {
      Intake.setVelocity(75, vex::percent);
    }
    else
    {
      Intake.setVelocity(0, vex::percent);
    }

    if (!Controller1.ButtonY.pressing())
    {
      RFM.spin(vex::forward);
      LFM.spin(vex::forward);
      LBM.spin(vex::forward);
      RBM.spin(vex::forward);
    }
    else
    {
      LFM.stop(vex::hold);
      RFM.stop(vex::hold);
      LBM.stop(vex::hold);
      RBM.stop(vex::hold);
    }

    //xComp = xComp * cos(InertSens.heading()*M_PI/180) + yComp * sin(InertSens.heading()*M_PI/180);
    //yComp = yComp * cos(InertSens.heading()*M_PI/180) + xComp * sin(InertSens.heading()*M_PI/180);

    RFM.setVelocity(xComp - yComp + rotComp, vex::percent);
    LFM.setVelocity(yComp - xComp + rotComp, vex::percent);
    RBM.setVelocity(rotComp - yComp - xComp, vex::percent);
    LBM.setVelocity(yComp + xComp + rotComp, vex::percent);
    wait(20, msec);

    
  }
}

int main()
{
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
  while (true)
  {
    wait(100, msec);
  }
}
