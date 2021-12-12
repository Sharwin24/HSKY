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
okapi::Motor rightLift(8);

// Controller
okapi::Controller controller;
ChassisControllerBuilder chassisBuilder = ChassisControllerBuilder();
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
	chassisBuilder.withMotors({2,5,7},{3,5,6});

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
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}


void setDrive(int left, int right) {
	driveLeftBackBack.moveVoltage(12000 * left);
	driveLeftBackFront.moveVoltage(12000 * left);
	driveLeftFront.moveVoltage(12000 * left);

	driveRightBackBack.moveVoltage(12000 * right);
	driveRightBackFront.moveVoltage(12000 * right);
	driveRightFront.moveVoltage(12000 * right);
}

void setDriveArcade(int left, int right) {
	int power = left;
	int turn = right;
	int l = power + turn;
	int r = power - turn;
	setDrive(l, r);
}

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
		pros::delay(20);
	}
}
