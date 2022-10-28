/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       Auton.h                                                   */
/*    Author:       Ellie Britt                                               */
/*    Created:      Sep 1, 2021                                               */
/*    Description:  Autonomous Methods and Routes                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#pragma once

#include <vex.h>
#include <math.h>
#include <string.h>

class Auton
{
/*-------------------------*/
/*    PRIVATE METHODS      */
/*-------------------------*/
private:
  // internal variables
  double rTempPos[4] = {0,0,0,0}; //LFM, RFM, LBM, RBM
  double rPos[3] = {0,0,0};  // Variable for robot local position [x,y,rot]

  double rFPos[2] = {0,0};  // Variable for final robot position [x,y]
  double fDelta[3] = {0,0,0};   // Variable for final robot field delta
  double fPos[3] = {0,0,0}; // Variable for final calculated position

  double tPos[3] = {0,0,0};   // set up target variables [x,y,rot]
 
  bool autonType = true;  // True - complicated, false - simple

  void reset_motor_pos()
  {
    RFM.setPosition(0, vex::deg);
    LFM.setPosition(0, vex::deg);
    RBM.setPosition(0, vex::deg);
    LBM.setPosition(0, vex::deg);
  }

  void kill_drive(bool vel)
  {
    if (vel)
    {
      LFM.setVelocity(0, vex::percent);
      RFM.setVelocity(0, vex::percent);
      LBM.setVelocity(0, vex::percent);
      RBM.setVelocity(0, vex::percent);
    }
    LFM.stop();
    RFM.stop();
    LBM.stop();
    RBM.stop();
  }

  double r_get_target(bool isAngle = true)
  {
    double dist[2] = {tPos[0]-fPos[0], tPos[1]-fPos[1]};
    if (isAngle)
      return atan2(dist[1], dist[0]);  // return the angle of the target
    else
      return sqrt(pow(dist[0],2) + pow(dist[1], 2));    // return the distance of the target
  }
  void r_motor_power(double vd, bool sp = false)
  {
    double tAngle = InertSens.heading(vex::degrees) - (r_get_target()*180/M_PI);
    // Filter the angle to work in increments of 360
    if (tAngle < 0)
      tAngle += 360;
    else if (tAngle > 360)
      tAngle -= 360;

    double xComp = vd * sin(tAngle*M_PI/180);
    double yComp = vd * cos(tAngle*M_PI/180);
    
    // Modified Driver control code. The rotation component isn't needed
    double rfmVM = xComp - yComp;
    double lfmVM = yComp - xComp;
    double rbmVM = -yComp - xComp;
    double lbmVM = yComp + xComp;

    LFM.setVelocity(100 * lfmVM, vex::percent);
    RFM.setVelocity(100 * rfmVM, vex::percent);
    LBM.setVelocity(100 * lbmVM, vex::percent);
    RBM.setVelocity(100 * rbmVM, vex::percent);

    if (sp)
    {
      LFM.spin(vex::forward);
      RFM.spin(vex::forward);
      LBM.spin(vex::forward);
      RBM.spin(vex::forward);
    }
  }
  void r_move(double tX, double tY, double margin, uint8_t tPrecision = 20)
  {
    tPos[0] += tX; // Set Targets for motion
    tPos[1] += tY;
    double tInitDist = r_get_target(false);
    r_motor_power(0.1, true); // Initial spin
    while (r_get_target(false) > margin)
    {
      r_motor_power(15*sin(M_PI*(r_get_target()/tInitDist)), false);
      wait(tPrecision, msec);
    }
    LFM.stop();
    RFM.stop();
    LBM.stop();
    RBM.stop();
  }

  void r_rotate(uint8_t margin)
  {
    double rDelta = tPos[2] - InertSens.heading(vex::deg);
    double rCDelta = rDelta;
    // If the change in rotation is positive, auto turn to the right
    if (rCDelta > 0)
    {
      while (rCDelta < margin)
      {
        rCDelta = tPos[2] - InertSens.heading(vex::deg);
        double mult = rCDelta/rDelta;
        LFM.setVelocity(sin(mult*M_PI)*100, vex::percent);
        RFM.setVelocity(sin(mult*M_PI)*100, vex::percent);
        LBM.setVelocity(sin(mult*M_PI)*100, vex::percent);
        RBM.setVelocity(sin(mult*M_PI)*100, vex::percent);
        LFM.spin(vex::forward);
        RFM.spin(vex::forward);
        LBM.spin(vex::forward);
        RBM.spin(vex::forward);
      }
      kill_drive(true);
    }
    // If the change in rotation is negative, auto turn to the left
    else if (rCDelta < 0)
    {
      while (rCDelta > -margin)
      {
        rCDelta = tPos[2] - InertSens.heading(vex::deg);
        double mult = rCDelta/rDelta;
        LFM.setVelocity(sin(mult*M_PI)*-100, vex::percent);
        RFM.setVelocity(sin(mult*M_PI)*-100, vex::percent);
        LBM.setVelocity(sin(mult*M_PI)*-100, vex::percent);
        RBM.setVelocity(sin(mult*M_PI)*-100, vex::percent);
        LFM.spin(vex::forward);
        RFM.spin(vex::forward);
        LBM.spin(vex::forward);
        RBM.spin(vex::forward);
      }
      kill_drive(true);
    }
  }

  void old_r (double vel, uint8_t dir, double deg)
  {
    double c_angle = InertSens.heading();
    switch(dir)
    {
      case 0:
        // Rotate Left
        InertSens.setHeading(0, vex::deg);
        LFM.setVelocity(-vel, vex::percent);
        RFM.setVelocity(-vel, vex::percent);
        LBM.setVelocity(-vel, vex::percent);
        RBM.setVelocity(-vel, vex::percent);
        LFM.spin(vex::forward);
        RFM.spin(vex::forward);
        LBM.spin(vex::forward);
        RBM.spin(vex::forward);
        wait(0.2, sec);
        while(InertSens.angle() > deg){ }
        kill_drive(true);
        break;

      case 1:
        // Rotate Right
        InertSens.setHeading(0, vex::deg);
        LFM.setVelocity(vel, vex::percent);
        RFM.setVelocity(vel, vex::percent);
        LBM.setVelocity(vel, vex::percent);
        RBM.setVelocity(vel, vex::percent);
        LFM.spin(vex::forward);
        RFM.spin(vex::forward);
        LBM.spin(vex::forward);
        RBM.spin(vex::forward);
        wait(0.2, sec);
        while(InertSens.angle() < deg) { }
        kill_drive(true);
        break;
    }
    InertSens.setHeading(0, degrees);
  }
  // Left Side Competition
  void auton_type_1()
  {
    LFM.stop(vex::hold);
    RFM.stop(vex::hold);
    LBM.stop(vex::hold);
    RBM.stop(vex::hold);
    DR4B_left.setVelocity(-90, vex::percent);
    DR4B_right.setVelocity(-90, vex::percent);
    DR4B_left.spin(vex::forward);
    DR4B_right.spin(vex::forward);
    wait(0.5, vex::seconds);
    DR4B_right.stop(vex::hold);
    DR4B_left.stop(vex::hold);

    Intake.setVelocity(-70, vex::percent);
    Intake.spin(vex::forward);
    wait(0.2, vex::seconds);
    Intake.stop();

    DR4B_left.setVelocity(60, vex::percent);
    DR4B_right.setVelocity(60, vex::percent);
    DR4B_left.spin(vex::forward);
    DR4B_right.spin(vex::forward);
    wait(0.5, vex::seconds);
    DR4B_right.stop(vex::hold);
    DR4B_left.stop(vex::hold);
    
    wait(0.7, vex::seconds);

    move_OLD(3, 25, 1.2, 1, 1);
    
    wait(0.3, vex::seconds);
    move_OLD(0, 100, 0.2, 2, 0.6);
    move_OLD(2, 10, 0);

    DR4B_left.setVelocity(-50, vex::percent);
    DR4B_right.setVelocity(-50, vex::percent);
    DR4B_left.spin(vex::forward);
    DR4B_right.spin(vex::forward);
    wait(0.5, vex::seconds);
    DR4B_right.stop(vex::hold);
    DR4B_left.stop(vex::hold);

    Intake.setVelocity(-30, vex::percent);
    Intake.spin(vex::forward);
    wait(0.5, vex::seconds);
    Intake.stop();
  }
  // Simple Auton for no win point
  void auton_type_2()
  {
    LFM.stop(vex::hold);
    RFM.stop(vex::hold);
    LBM.stop(vex::hold);
    RBM.stop(vex::hold);
    DR4B_left.setVelocity(-90, vex::percent);
    DR4B_right.setVelocity(-90, vex::percent);
    DR4B_left.spin(vex::forward);
    DR4B_right.spin(vex::forward);
    wait(0.5, vex::seconds);
    DR4B_right.stop(vex::hold);
    DR4B_left.stop(vex::hold);

    Intake.setVelocity(-50, vex::percent);
    Intake.spin(vex::forward);
    wait(1.2, vex::seconds);
    Intake.stop();

    //move_OLD(0, 100, 0.2, 2, 0.6);

    //move_OLD(3, 25, 1.2, 1, 1);

    //move_OLD(0, -100, 0.2, 1.8, 0.6);
  }

  void dir_OLD(uint8_t dir, double vel)
  {
    switch(dir)
    {
      case 0:
        LFM.setVelocity(vel, vex::percent);
        RFM.setVelocity(-vel, vex::percent);
        LBM.setVelocity(vel, vex::percent);
        RBM.setVelocity(-vel, vex::percent);
        break;
      case 1:
        LFM.setVelocity(-vel, vex::percent);
        RFM.setVelocity(vel, vex::percent);
        LBM.setVelocity(-vel, vex::percent);
        RBM.setVelocity(vel, vex::percent);
        break;
      case 2:
        LFM.setVelocity(-vel, vex::percent);
        RFM.setVelocity(vel, vex::percent);
        LBM.setVelocity(vel, vex::percent);
        RBM.setVelocity(-vel, vex::percent);
        break;
      case 3:
        LFM.setVelocity(vel, vex::percent);
        RFM.setVelocity(-vel, vex::percent);
        LBM.setVelocity(-vel, vex::percent);
        RBM.setVelocity(vel, vex::percent);
        break;
    }
    LFM.spin(vex::forward);
    RFM.spin(vex::forward);
    LBM.spin(vex::forward);
    RBM.spin(vex::forward);
  }
  void move_OLD(uint8_t dir, double vel, double timeS, double timeup = 1, double timedown = 1)
  {
    double wtimeU = timeup/10;
    double wtimeD = timedown/10;
    double currentvel = 0;
    for (int i = 0 ; i < 10 ; i++)
    {
      currentvel += vel/10;
      dir_OLD(dir, currentvel);
      wait(wtimeU, vex::seconds);
    }
    wait(timeS, vex::seconds);
    for (int i = 0 ; i < 10 ; i++)
    {
      currentvel -= vel/10;
      dir_OLD(dir, currentvel);
      wait(wtimeU, vex::seconds);
    }
    kill_drive(true);
  }

/*-------------------------*/
/*    PUBLIC METHODS       */
/*-------------------------*/
public:
  vex::task position_scan()
  {
    while (true) {
      // Calculate linear distance in inches
      rTempPos[0] = (1.625*LFM.position(vex::deg)*M_PI)/450;
      rTempPos[1] = (1.625*RFM.position(vex::deg)*M_PI)/450;
      rTempPos[2] = (1.625*LBM.position(vex::deg)*M_PI)/450;
      rTempPos[3] = (1.625*RBM.position(vex::deg)*M_PI)/450;
      Brain.Screen.clearLine();
      Brain.Screen.print(fPos[0]);
      Brain.Screen.print(fPos[1]);

      // Average the 2 positions into local wheel axis
      rPos[0] = (-rTempPos[0] - rTempPos[3]) / 2;  // X axis AVG of LFM & RBM
      rPos[1] = (-rTempPos[1] + rTempPos[2]) / 2;  // Y axis AVG of RFM & LBM

      // Use trig to convert to local robot axis
      rFPos[0] = rPos[0]/cos(M_PI/4);
      rFPos[1] = rPos[1]/cos(M_PI/4);

      // Calculate final position delta
      rPos[2] = InertSens.heading(vex::deg);
      fDelta[0] = rFPos[0]*cos(rPos[2]*M_PI/180);
      fDelta[1] = rFPos[1]*cos(rPos[2]*M_PI/180);

      // Add to final field position
      fPos[0] += fDelta[0];
      fPos[1] += fDelta[1];
    
      // Reset the encoder positions for the next calculations
      reset_motor_pos();
      wait(10, vex::msec);
    }
  }

  void auton_select()
  {
    if (autonType)
      auton_type_1();
    else
      auton_type_2();
  }

  void skills_autonomous()
  {

  }
};