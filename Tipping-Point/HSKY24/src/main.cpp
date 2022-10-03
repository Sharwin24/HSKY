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

pid GY;
pid US;

// Int to store conveyorbelt state
int conveyorbelt_state = 0;
// int to store tilter state
int tilter_state = 0;
// int to store clamp state
int clamp_state = 0;

okapi::Motor clampMotor(8, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts); // TODO:: Will change to red at tourney
okapi::Motor conveyorbeltMotor(19, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor tilterMotor(6, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);

// Motor Setup
okapi::Motor driveRightBackFront(12, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveRightBackBack(4, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveRightFront(11, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftBackBack(9, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftBackFront(18, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftFront(20, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);

// Doing controller things
okapi::Controller controller;
auto chassis = ChassisControllerBuilder().withMotors({9, 20, -18}, {-11, -4, 12}).withDimensions(AbstractMotor::gearset::green, {{4.125_in, 13_in}, imev5GreenTPR}).build();
auto model = chassis -> getModel();

pros::IMU imu(5);

#define ULTRA_PING_PORT 'H'
#define ULTRA_ECHO_PORT 'G'

pros::ADIUltrasonic ultrasonic(ULTRA_PING_PORT, ULTRA_ECHO_PORT);

void gyroPID(int rotation)
{
	GY.target = imu.get_rotation() + rotation;
	GY.integral = 0;
	bool val = false;
	int timer = 0;
	while (timer < 50)
	{
		GY.kP = 0.4;
		GY.kD = 0.1;
		GY.kI = 0.00004;
		GY.sensor = imu.get_rotation();
		//std::cout << "POS: " << GY.sensor << std::endl;
		GY.error = GY.target - GY.sensor;
		GY.derivative = GY.error - GY.previous_error;
		GY.integral += GY.error;
		GY.previous_error = GY.error;
		GY.speed = (GY.kP * GY.error + GY.kD * GY.derivative + GY.kI * GY.integral) / 30;

		model->tank(GY.speed, -1 * GY.speed);

		timer++;
		pros::delay(20);
		pros::lcd::set_text(1, "Gyro: " + std::to_string(GY.sensor));
	}
	model->tank(0, 0);
}

const int CLAMP_TIME = 250;
bool moveClamp(bool up)
{
	int speed = 100;
	if (up)
	{
		clampMotor.moveVelocity(speed);
	}
	else
	{
		clampMotor.moveVelocity(-speed);
	}
	pros::delay(CLAMP_TIME);
	clampMotor.moveVelocity(0);
	return true;
}

const int num_count = 290;
bool moveTilter(bool up)
{
	if (up)
	{
		tilterMotor.moveRelative(300 * 3, 100);
	}
	else
	{
		tilterMotor.moveRelative(-290 * 3, 100);
	}
	return true;
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
	imu.reset();
	while (imu.is_calibrating())
	{
		pros::delay(10);
		pros::lcd::set_text(1, "Calibrating");
	}

	pros::delay(2000);
	// pros::lcd::register_btn1_cb(on_center_button);
	clampMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
	tilterMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
	conveyorbeltMotor.setBrakeMode(AbstractMotor::brakeMode::coast);

	// Moves the tiler motor down to the bottom (preps for pickup)
	moveTilter(false);
	moveClamp(true);
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

// TODO Pid needs tuning
double pGain = 0.0012;
double iGain = 0.00000;
double dGain = 0.00000;

void movePID(double distanceL, double distanceR, int ms, double maxV = 1)
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
		model->tank(powerL * maxV, powerR * maxV);
		std::cout << "Left: " << model->getSensorVals()[0] << " Right: " << model->getSensorVals()[1] << std::endl;
		std::cout << errorL << " " << errorR << std::endl;

		pros::delay(10);
		timer += 10;
	}

	model->tank(0, 0);
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
		bool tilterUp = controller.getDigital(ControllerDigital::L1);
		bool tilterDown = controller.getDigital(ControllerDigital::L2);
		if (tilterUp && tilterDown)
		{
			tilterMotor.moveVelocity(0);
		}
		else if (tilterUp)
		{
			tilterMotor.moveVelocity(100);
		}
		else if (tilterDown)
		{
			tilterMotor.moveVelocity(-100);
		}
		else
		{
			tilterMotor.moveVelocity(0);
		}

		// Now we need to go ahead and set up the movement for the conveyor
		// Simple toggle should do based on a and b to enable and disable
		// As of right now we shouldn't need the ability to reverse it, could be changed in the future
		bool aPressed = controller.getDigital(ControllerDigital::A);
		bool bPressed = controller.getDigital(ControllerDigital::B);
		bool yPressed = controller.getDigital(ControllerDigital::Y);

		if (aPressed && !conveyorbelt_state)
		{
			conveyorbelt_state = 1;
		}
		else if (bPressed && conveyorbelt_state)
		{
			conveyorbelt_state = 0;
		}

		// If the y button is held it will override the current state
		if (yPressed)
		{
			conveyorbeltMotor.moveVelocity(200);
		}
		else // if the y button is not helpd then it will revert to default behavior as governed by the state
		{
			conveyorbeltMotor.moveVelocity(conveyorbelt_state * -200);
		}

		bool clampUp = controller.getDigital(ControllerDigital::R1);
		bool clampDown = controller.getDigital(ControllerDigital::R2);
		if (clampUp && clampDown)
		{
			clampMotor.moveVelocity(0);
		}
		else if (clampUp)
		{
			clampMotor.moveVelocity(100);
		}
		else if (clampDown)
		{
			clampMotor.moveVelocity(-100);
		}
		else
		{
			clampMotor.moveVelocity(0);
		}
		if (controller.getDigital(ControllerDigital::left))
		{
			gyroPID(-90);
		}
		else if (controller.getDigital(ControllerDigital::right))
		{
			gyroPID(90);
		}

		// if (controller.getDigital(ControllerDigital::X))
		// {
		// 	autonomous();
		// }

		pros::delay(20);
		double val = ultrasonic.get_value();
		double heading = imu.get_heading();
		double rotation = imu.get_rotation();

		pros::lcd::set_text(1, "Ultrasonic: " + std::to_string(val));
		pros::lcd::set_text(2, "Heading: " + std::to_string(heading));
		pros::lcd::set_text(3, "Rotation: " + std::to_string(rotation));
		pros::lcd::set_text(5, "Pitch: " + std::to_string(imu.get_pitch()));
	}
}

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
void autonomous()
{
	moveClamp(true);
	moveTilter(false);
	// Moves forward
	movePID(44, 44, 1200);
	// Grabs the yello mobile goal
	moveClamp(false);
	// Moves backwards
	movePID(-40, -40, 1500);
	// Turns left 45
	gyroPID(-45);
	// Moves forward
	movePID(10, 10, 1000);
	// Drops yellow goal
	moveClamp(true);
	// Moves backwards
	movePID(-5, -5, 2000);
	// Rotates 90
	gyroPID(90);
	// Moves forward
	movePID(9, 9, 1000);
	// Grabs home mobile goal
	moveClamp(false);
	pros::delay(500);
	// Picks up home mobile goal
	moveTilter(true);
	// Moves backwards with goal
	movePID(-7, -7, 1000);
	// Rotates back to face the wall and pick up rings
	gyroPID(178 - imu.get_rotation());
	//turns converyor on
	conveyorbeltMotor.moveVelocity(-150);
	pros::delay(2000);
	movePID(-4, -4, 3000);
}