#include "main.h"
#include "api.h"
#include "okapi/api.hpp"
using namespace okapi;

struct PID
{
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

int LIGHT_THRESHHOLD = 2500;
bool intakeBall = false;
bool indexerBall = false;
int taskChoice = 0;
int indexerToggle = 0;
int flywheelToggle = 0;

// Motor Setup
okapi::Motor driveRightBackBack(-5);
okapi::Motor driveRightBackFront(4);
okapi::Motor driveRightFront(-11);
okapi::Motor driveLeftBackBack(8);
okapi::Motor driveLeftBackFront(-7);
okapi::Motor driveLeftFront(20);

okapi::Motor rightLift = Motor(-3);
okapi::Motor leftLift = Motor(2);

okapi::Motor clamp(6);

// Doing controller things
okapi::Controller controller;
auto chassis = ChassisControllerBuilder().withMotors({8, -7, 20}, {-5, 4, -11}).withDimensions(AbstractMotor::gearset::green, {{4.125_in, 9.5_in}, imev5GreenTPR}).build();
auto model = chassis -> getModel();
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
	static bool pressed = false;
	pressed = !pressed;
	if (pressed)
	{
		pros::lcd::set_text(2, "I was pressed!");
	}
	else
	{
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");
	pros::lcd::register_btn1_cb(on_center_button);
	clamp.setBrakeMode(AbstractMotor::brakeMode::hold);
	leftLift.setBrakeMode(AbstractMotor::brakeMode::hold);
	rightLift.setBrakeMode(AbstractMotor::brakeMode::hold);
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

const int CLAMP_TIME = 100;

bool moveClamp(bool up)
{
	int speed = 200;
	if (up)
	{
		clamp.moveVelocity(-speed);
	}
	else
	{
		clamp.moveVelocity(speed);
	}
	pros::delay(CLAMP_TIME);
	return true;
}

double pGain = 0.00185;
double iGain = 0.000;
double dGain = 0.000;

void movePID(double distanceL, double distanceR, int ms)
{
	double degreesL = distanceL / (PI * 4.125) * 360;
	double degreesR = distanceR / (PI * 4.125) * 360;
	auto drivePIDL = okapi::IterativeControllerFactory::posPID(pGain, iGain, dGain);
	auto drivePIDR = okapi::IterativeControllerFactory::posPID(pGain, iGain, dGain);
	std::cout << drivePIDL.getSampleTime().convert(okapi::millisecond) << std::endl;
	model->resetSensors();

	int timer = 0;
	double errorL;
	double errorR;
	double powerL;
	double powerR;
	while (timer < ms)
	{
		errorL = degreesL + model->getSensorVals()[0] / 900 * 360;
		errorR = degreesR + model->getSensorVals()[1] / 900 * 360;
		powerL = drivePIDL.step(errorL);
		powerR = drivePIDR.step(errorR);
		model->tank(powerL, powerR);
		std::cout << "Left: " << model->getSensorVals()[0] << " Right: " << model->getSensorVals()[1] << std::endl;
		std::cout << errorL << " " << errorR << std::endl;

		pros::delay(10);
		timer += 10;
	}

	model->tank(0, 0);
}

void turnPid(double degrees, bool right)
{
	// Need to calculate what a degree change in the robot is based on wheel rotation
	// If we have an 11.5 inch wheel distance then we can approximate that the robot will rotate on a circle with radius 11.5 / sqrt(2) = 7.07

	// Now if we want to rotate in place we can simply rotate half of that distance with the right wheels while doing the other half with the left wheels

	// The circumfrence is 2 * PI * 7.07
	double circ = 2 * PI * 7.07;
	double distance = circ / 360 * degrees;
	double distanceL = distance / 2;
	double distanceR = distance / 2;
	if (right)
	{
		distanceL = -distanceL;
	}
	else
	{
		distanceR = -distanceR;
	}
	movePID(distanceL, distanceR, 1000);
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
void opcontrol()
{
	int clampInUpPosition = 1;
	while (true)
	{
		// Ok so this is gonna do the arcade moement for the actual robot
		chassis->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
									controller.getAnalog(ControllerAnalog::rightX));
		// Now we need to go ahead and set up the movement for the lift
		// So if we go ahead and grab that information and pipe it to the okapi motor as a velocity
		bool up = controller.getDigital(ControllerDigital::L1);
		bool down = controller.getDigital(ControllerDigital::L2);
		if (up && down)
		{
			leftLift.moveVelocity(0);
			rightLift.moveVelocity(0);
		}
		else if (up)
		{
			leftLift.moveVelocity(-200);
			rightLift.moveVelocity(-200);
		}
		else if (down)
		{
			leftLift.moveVelocity(200);
			rightLift.moveVelocity(200);
		}
		else
		{
			leftLift.moveVelocity(0);
			rightLift.moveVelocity(0);
		}

		bool clampUp = controller.getDigital(ControllerDigital::R1);
		bool clampDown = controller.getDigital(ControllerDigital::R2);
		if (clampUp && clampDown)
		{
			clamp.moveVelocity(0);
		}
		else if (clampUp)
		{
			clamp.moveVelocity(-200);
		}
		else if (clampDown)
		{
			clamp.moveVelocity(200);
		}
		else
		{
			clamp.moveVelocity(0);
		}

		// // Finally we need to set up the clamp button
		// bool clampUp = controller.getDigital(ControllerDigital::R1);
		// bool clampDown = controller.getDigital(ControllerDigital::R2);
		// bool moved = false;
		// if (clampUp && clampDown)
		// {
		// }
		// else if (clampUp && !clampInUpPosition)
		// {
		// 	moved = moveClamp(false);
		// }
		// else if (clampDown && clampInUpPosition)
		// {
		// 	moved = moveClamp(true);
		// }
		// else
		// {
		// 	clamp.moveVelocity(0);
		// }

		// if (moved)
		// {
		// 	clampInUpPosition = !clampInUpPosition;
		// }

		if (controller.getDigital(ControllerDigital::up))
		{
			movePID(-40, -40, 1500);
		}
		else if (controller.getDigital(ControllerDigital::down))
		{
			movePID(40, 40, 1500);
		}
		else if (controller.getDigital(ControllerDigital::left))
		{
			turnPid(90, false);
		}
		else if (controller.getDigital(ControllerDigital::right))
		{
			turnPid(90, true);
		}

		std::cout << model->getSensorVals()[0] << " " << model->getSensorVals()[1] << std::endl;

		pros::delay(20);
	}
}