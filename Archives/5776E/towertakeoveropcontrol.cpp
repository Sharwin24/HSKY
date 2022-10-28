#include "main.h"
using namespace okapi;
struct PID {
  float kP;
  float kI;
  float kD;
  float integral;
  float derivative;
  float error;
  float previous_error;
  float speed;
  float target;
  float sensor;
};
typedef struct PID pid;
pid LT;
// others

// pros::ADIPotentiometer potCollector('E');
okapi::Controller controller;
okapi::Motor backLift(-8);
okapi::Motor armLift(14);
okapi::MotorGroup rollers({12, -19});
int lcdCounter = 1;
int intakeSpeed = 0;
int buttonCount = 0;
bool isPressed = false;

auto chassis = okapi::ChassisControllerFactory::create(
    {1, 11}, {-10, -20}, okapi::AbstractMotor::gearset::green, {4.125, 10});
// auto motorGroup = okapi::ChassisControllerFactory::create({3,-10},
// okapi::AbstractMotor::gearset::green,{4.125,10}); NEED PORT auto lift =
// okapi::ChassisControllerFactory::create() Position Tracking start float
// dTheta; //= (renc - lenc)/chaswidth -> radians float sector; //sector =
// (renc+lence)/2 float radius; //radius = sector/dTheta End

void backLiftPID(double degrees) {
  LT.target = degrees;
  LT.integral = 0;
  LT.sensor = backLift.getPosition();
  LT.error = LT.target - LT.sensor;
  int timer = 0;

  while (abs(LT.error) >= 10) {         // or while(timer < 50){
    LT.kP = 0.3;                        // need tuning
    LT.kD = 0;                          // need tuning
    LT.kI = 0;                          // need tuning
    LT.sensor = backLift.getPosition(); // = sensor.getValue || post setup
    LT.error = LT.target - LT.sensor;
    LT.derivative = LT.error - LT.previous_error;
    LT.integral += LT.error;
    LT.previous_error = LT.error;
    LT.speed = (LT.kP * LT.error + LT.kD * LT.derivative + LT.kI * LT.integral);
    backLift.moveVelocity(LT.speed);
    // fill
    timer++;
    pros::delay(20);
  }
}

void armLiftPID(double degrees) {
  LT.target = degrees;
  LT.integral = 0;
  LT.sensor = armLift.getPosition();
  LT.error = LT.target - LT.sensor;
  int timer = 0;

  while (abs(LT.error) >= 10) {        // or while(timer < 50){
    LT.kP = 0.4;                       // need tuning
    LT.kD = 0.1;                       // need tuning
    LT.kI = 0;                         // need tuning
    LT.sensor = armLift.getPosition(); // = sensor.getValue || post setup
    LT.error = LT.target - LT.sensor;
    LT.derivative = LT.error - LT.previous_error;
    LT.integral += LT.error;
    LT.speed = (LT.kP * LT.error + LT.kD * LT.derivative + LT.kI * LT.integral);
    armLift.moveVelocity(LT.speed);
    // fill
    timer++;
    pros::delay(20);
  }
}

void movePID(double distanceL, double distanceR, int ms) {
  double targetL = distanceL * 360 / (2 * 3.1415 * (4.125 / 2));
  double targetR = distanceR * 360 / (2 * 3.1415 * (4.125 / 2));
  auto drivePIDL =
      okapi::IterativeControllerFactory::posPID(0.0016, 0.001, 0.0015); //= data
  auto drivePIDR =
      okapi::IterativeControllerFactory::posPID(0.0016, 0.001, 0.0015);
  chassis.resetSensors();

  int timer = 0;
  double errorL;
  double errorR;
  double powerL;
  double powerR;

  while (timer < ms) {
    errorL = targetL - chassis.getSensorVals()[0];
    errorR = targetR - chassis.getSensorVals()[1];
    powerL = drivePIDL.step(errorL);
    powerR = drivePIDR.step(errorR);
    chassis.tank(-powerL, -powerR);

    pros::delay(10);
    timer += 10;
  }

  chassis.tank(0, 0);
}

void opcontrol() {
  while (true) {
    // std::cout << intakeLS.get_value() << " " << indexerLS.get_value() << " "
    // << hoodLS.get_value() << " " << intakeBall << " " << indexerBall << " "
    // << hoodBall << std::endl;
    chassis.arcade(controller.getAnalog(ControllerAnalog::leftY),
                   controller.getAnalog(ControllerAnalog::rightX));
    backLift.moveVelocity(25 * controller.getDigital(ControllerDigital::R1) -
                          25 * controller.getDigital(ControllerDigital::R2));
    armLift.moveVelocity(200 * controller.getDigital(ControllerDigital::up) -
                         200 * controller.getDigital(ControllerDigital::down));
    /*if (backLift.getPosition() > -500 &&
    controller.getDigital(ControllerDigital::up) ||
    controller.getDigital(ControllerDigital::down)) { backLift.moveVelocity(50 *
    controller.getDigital(ControllerDigital::up) - 50 *
    controller.getDigital(ControllerDigital::down));
    }*/
    // intakeSpeed = 200;
    std::cout << buttonCount << " -- " << intakeSpeed << std::endl;
    if (controller.getDigital(ControllerDigital::left)) {

      buttonCount++;
      if (buttonCount % 2 == 1) {
        intakeSpeed = 100;
      } else {
        intakeSpeed = 200;
      }
      pros::delay(300);
    }

    rollers.moveVelocity(
        intakeSpeed * controller.getDigital(ControllerDigital::L1) -
        intakeSpeed * controller.getDigital(ControllerDigital::L2));
    // rollers.moveVelocity(200 * controller.getDigital(ControllerDigital::L1) -
    // 200 * controller.getDigital(ControllerDigital::L2));
    // motorGroup.arcade(200 * controller.getDigital(ControllerDigital::L1));
    // leftRoller.moveVelocity(200 *
    // controller.getDigital(ControllerDigital::L1));
    // rightRoller.moveVelocity(200 *
    // controller.getDigital(ControllerDigital::R1)); indexer.moveVelocity(200 *
    // controller.getDigital(ControllerDigital::L1) - 200 *
    // controller.getDigital(ControllerDigital::L2)); flipper.moveVelocity(200 *
    // controller.getDigital(ControllerDigital::up) - 200 *
    // controller.getDigital(ControllerDigital::down));

    pros::delay(20);
  }
}

// void
/*void positionTracking(double x, double y){
        int rect = 0;
        int length = 0;
        int chaswidth = 0;
}*/

// Autonomous
void red() {
  // Flip out
  // backLiftPID(760);
  backLift.moveVelocity(200);
  pros::delay(1000);
  backLift.moveVelocity(0);
  armLiftPID(425);
  armLift.moveVelocity(-100);
  pros::delay(500);
  armLift.moveVelocity(0);
  backLiftPID(0);
  pros::delay(25);

  // Pick up the cubes
  rollers.moveVelocity(200);
  movePID(44, 44, 3000);

  // Move back to wall align
  movePID(-44, -44, 3000);
  rollers.moveVelocity(0);
  ;

  // Move forward, turn, and into goal
  movePID(10, 10, 1500);
  movePID(16, -16, 1500);
  movePID(18, 18, 1000);

  // Get bottom cube in position to stack
  rollers.moveVelocity(-100);
  pros::delay(600);
  rollers.moveVelocity(0);

  // Straighten up the tray and align bottom
  backLiftPID(800);
  backLift.moveVelocity(0);

  // Outtake the cubes and move backwards
  rollers.moveVelocity(-100);
  movePID(-15, -15, 1500);
  rollers.moveVelocity(0);
}

void push() {
  movePID(30, 30, 1500);
  movePID(-30, -30, 1500);
}

void blue() {
  // Flip out
  backLift.moveVelocity(200);
  pros::delay(1500);
  backLift.moveVelocity(0);
  armLiftPID(425);
  armLift.moveVelocity(-100);
  pros::delay(500);
  armLift.moveVelocity(0);
  backLiftPID(0);
  pros::delay(25);

  // Pick up the cubes
  rollers.moveVelocity(200);
  movePID(44, 44, 3000);

  // Move back to wall align
  movePID(-44, -44, 3000);
  rollers.moveVelocity(0);
  ;

  // Move forward, turn, and into goal
  movePID(10, 10, 1500);
  movePID(-16, 16, 1500);
  movePID(18, 18, 1000);

  // Get bottom cube in position to stack
  rollers.moveVelocity(-100);
  pros::delay(600);
  rollers.moveVelocity(0);

  // Straighten up the tray and align bottom
  backLiftPID(800);
  backLift.moveVelocity(0);

  // Outtake the cubes and move backwards
  rollers.moveVelocity(-100);
  movePID(-15, -15, 1500);
  rollers.moveVelocity(0);
}

void autonomous() {
  switch (lcdCounter) {
  case 0:
    break;
  case 1:
    red();
    break;
  case 2:
    push();
    break;
  case 3:
    blue();
    break;
  }
}

bool selected = true; // TODO: false

void left_button() {
  if (!selected) {
    lcdCounter--;
    if (lcdCounter < 0) {
      lcdCounter = 0;
    }
  }
}
void center_button() {
  if (!selected) {
    selected = true;
  }
}
void right_button() {
  if (!selected) {
    lcdCounter++;
    if (lcdCounter > 3) {
      lcdCounter = 3;
    }
  }
}
std::string convert(int arg) {
  switch (arg) {
  case 0:
    return "No Auton";
  case 1:
    return "Red";
  case 2:
    return "Push";
  case 3:
    return "Blue";
  default:
    return "No Auton";
  }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

void initialize() {
  pros::lcd::initialize();
  pros::lcd::register_btn0_cb(left_button);
  pros::lcd::register_btn1_cb(center_button);
  pros::lcd::register_btn2_cb(right_button);

  // intakeLS.calibrate();
  // rollers.calibrate();
  // indexerLS.calibrate();
  // hoodLS.calibrate();

  while (!selected) {
    pros::lcd::set_text(0, convert(lcdCounter));
    pros::delay(20);
  }

  pros::lcd::set_text(0, convert(lcdCounter) + " (SELECTED)");
}

// void disabled() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

// void competitionitialize() {}