#include "main.h"
#include "api.h"
#include "okapi/api.hpp"

using namespace okapi;


// Motor Setup
okapi::Motor driveRightBackBack(-6);
okapi::Motor driveRightBackFront(4);
okapi::Motor driveRightFront(-3);
okapi::Motor driveLeftBackBack(7);
okapi::Motor driveLeftBackFront(-5);
okapi::Motor driveLeftFront(2);

okapi::Motor mobileGoalLift(10);

okapi::Motor leftLift(9);
okapi::Motor rightLift(-8);

// Controller
okapi::Controller controller;
//ChassisControllerBuilder chassisBuilder = ChassisControllerBuilder();
ControllerButton gDown(ControllerDigital::R1);
ControllerButton gUp(ControllerDigital::R2);
ControllerButton lDown(ControllerDigital::L1);
ControllerButton lUp(ControllerDigital::L2);

std::shared_ptr<ChassisController> chassis;

std::shared_ptr<AsyncPositionController<double, double>> liftController;

std::shared_ptr<AsyncPositionController<double, double>> mobileGoalLiftController;

// okapi::ChassisModel model = SkidSteerModel();
// auto chassis = okapi::ChassisControllerBuilder().withMotors(1,2);

// auto chassis = okapi::ChassisControllerFactory::create(
// 	{2,5,7},
// 	{3,5,6},
// 	okapi::AbstractMotor::gearset::green,
// 	{4.125,10}
// );


/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
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
	pros::lcd::set_text(1, "Hello PROS User!");
	pros::lcd::register_btn1_cb(on_center_button);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	chassis = ChassisControllerBuilder()
	.withMotors({2,5,7},{3,5,6})
	.withDimensions(AbstractMotor::gearset::red, {{4_in, 11_in}, imev5RedTPR})
	.build();

	const double liftkP = 0.001;
	const double liftkI = 0.0001;
	const double liftkD = 0.0001;

	liftController =
	  AsyncPosControllerBuilder()
	    .withMotor({8,9})
	    .withGains({liftkP, liftkI, liftkD})
	    .build();

	const double gLiftkP = 0.001;
	const double gLiftkI = 0.0001;
	const double gLiftkD = 0.0001;

	mobileGoalLiftController = AsyncPosControllerBuilder()
		.withMotor(10)
		.withGains({gLiftkP, gLiftkI, gLiftkD})
		.build();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the  robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	chassis->moveDistanceAsync(-1_ft);
	//liftController->setTarget(200);
	mobileGoalLiftController->setTarget(3000);
	//chassis->turnAngle(90_deg);
	//chassis->moveDistance(0.33_m);
}

// https://okapilib.github.io/OkapiLib/md_docs_tutorials_walkthrough_asyncAutonomousMovement.html

void setDrive(float left, float right) {
	driveLeftBackBack.moveVoltage(12000 * left);
	driveLeftBackFront.moveVoltage(12000 * left);
	driveLeftFront.moveVoltage(12000 * left);

	driveRightBackBack.moveVoltage(12000 * right);
	driveRightBackFront.moveVoltage(12000 * right);
	driveRightFront.moveVoltage(12000 * right);
}

/*
const int LIFT_MOTOR_PORT = 1; // Controlling a lift with a single motor on port 1

const int NUM_HEIGHTS = 4;
const int height1 = 20;
const int height2 = 60;
const int height3 = 100;
const int height4 = 140;

const int heights[NUM_HEIGHTS] = {height1, height2, height3, height4};

ControllerButton btnUp(ControllerDigital::R1);
ControllerButton btnDown(ControllerDigital::R2);
std::shared_ptr<AsyncPositionController<double, double>> liftControl =
  AsyncPosControllerBuilder().withMotor(LIFT_MOTOR_PORT).build();

void opcontrol() {
  int goalHeight = 0;

  while (true) {
    if (btnUp.changedToPressed() && goalHeight < NUM_HEIGHTS - 1) {
      // If the goal height is not at maximum and the up button is pressed, increase the setpoint
      goalHeight++;
      liftControl->setTarget(heights[goalHeight]);
    } else if (btnDown.changedToPressed() && goalHeight > 0) {
      goalHeight--;
      liftControl->setTarget(heights[goalHeight]);
    }

    pros::delay(10);
  }
}
*/

void setLift() {
	if (lUp.isPressed()) {
      leftLift.moveVoltage(6000);
			rightLift.moveVoltage(6000);
    } else if (lDown.isPressed()) {
			leftLift.moveVoltage(-6000);
			rightLift.moveVoltage(-6000);
    } else {
			leftLift.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
      leftLift.moveVoltage(0);
			rightLift.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
			rightLift.moveVoltage(0);
    }
}

void setGoalLift() {
	if (gUp.isPressed()) {
      mobileGoalLift.moveVoltage(12000);
    } else if (gDown.isPressed()) {
			mobileGoalLift.moveVoltage(-12000);
    } else {
			mobileGoalLift.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
      mobileGoalLift.moveVoltage(0);
    }
}

void setDriveArcade(float left, float right) {
	float power = left;
	float turn = right;
	float l = power + turn;
	float r = power - turn;
	setDrive(l, r);
}

/*
void setLift(int power) {
    leftLift = power;
    rightLift = power;
}

void setLiftMotors() {
    int power = 127 * (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)
                           - controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2));
    setLift(power);
}
*/

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
void opcontrol() {
	while (true) {
		//setDrive(1, 1);
		setDriveArcade(controller.getAnalog(ControllerAnalog::leftY),controller.getAnalog(ControllerAnalog::rightX));
		setLift();
		setGoalLift();
		pros::delay(10);

	}
}
