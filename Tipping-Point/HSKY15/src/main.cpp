#include "main.h"
#include "api.h"
#include "okapi/api.hpp"
using namespace okapi;

#define CLAMP_TIME 500

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
pid LIFT;

// Motor Setup
okapi::Motor driveRightBackBack(5, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveRightBackFront(4, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveRightFront(11, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftBackBack(8, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftBackFront(7, true, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor driveLeftFront(19, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::counts);

okapi::Motor rightLift = Motor(20, false, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);
okapi::Motor leftLift = Motor(2, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);

okapi::Motor clamp(18, true, okapi::AbstractMotor::gearset::red, okapi::AbstractMotor::encoderUnits::counts);

// DEAD PORTS: 1, 6, 9, 10, 21, 13, 3
#define ULTRA_PING_PORT 'A'
#define ULTRA_ECHO_PORT 'B'
#define POT_PORT 'H'

pros::ADIUltrasonic ultrasonic(ULTRA_PING_PORT, ULTRA_ECHO_PORT);
okapi::Potentiometer liftPot(POT_PORT);
pros::IMU imu(17);

// Doing controller things
okapi::Controller controller;
auto chassis = ChassisControllerBuilder().withMotors({8, -7, 19}, {-5, 4, -11}).withDimensions(AbstractMotor::gearset::green, {{4.125_in, 9.5_in}, imev5GreenTPR}).build();
auto model = chassis -> getModel();
void gyroPID(int rotation);

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
void autonomus();

bool moveClamp(bool up)
{
	int speed = 100;
	if (up)
	{
		clamp.moveVelocity(speed);
	}
	else
	{
		clamp.moveVelocity(-speed);
	}
	pros::delay(CLAMP_TIME);
	clamp.moveVelocity(0);
	return true;
}

double pGain = 0.00185;
double iGain = 0.000;
double dGain = 0.000;

void movePID(double distanceL, double distanceR, int ms, double maxV = 1)
{
	double degreesL = (distanceL * 360.0) / (3.1415 * 4.125);
	double degreesR = (distanceR * 360.0) / (3.1415 * 4.125);
	auto drivePIDL = okapi::IterativeControllerFactory::posPID(pGain, iGain, dGain);
	auto drivePIDR = okapi::IterativeControllerFactory::posPID(pGain, iGain, dGain);
	model->resetSensors();

	int timer = 0;
	double errorL;
	double errorR;
	double powerL;
	double powerR;
	while (timer < ms)
	{
		errorL = degreesL + model->getSensorVals()[0] * 360.0 / 900.0;
		errorR = degreesR + model->getSensorVals()[1] * 360.0 / 900.0;
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

void turnPid(double degrees, bool right)
{
	// Need to calculate what a degree change in the robot is based on wheel rotation
	// If we have an 11.5 inch wheel distance then we can approximate that the robot will rotate on a circle with radius 11.5 / sqrt(2) = 7.07

	// Now if we want to rotate in place we can simply rotate half of that distance with the right wheels while doing the other half with the left wheels

	// The circumfrence is 2 * PI * 7.07x

	double circ = 2.0 * 3.1415 * 7.07;
	double distanceT = circ / 360.0 * degrees;
	double distanceTL = distanceT;
	double distanceTR = distanceT;
	pros::lcd::set_text(1, "Degrees: " + std::to_string(degrees));
	pros::lcd::set_text(2, "Distance: " + std::to_string(distanceT));
	pros::lcd::set_text(3, "DistanceR: " + std::to_string(distanceTR));
	pros::lcd::set_text(4, "DistanceL: " + std::to_string(distanceTL));
	if (right)
	{
		distanceTL = -1 * distanceTL;
	}
	else
	{
		distanceTR = -1 * distanceTR;
	}
	movePID(distanceTL, distanceTR, 5000);
}

void gyroPID(int rotation)
{
	GY.target = rotation;
	imu.set_rotation(0.0);
	GY.integral = 0;
	bool val = false;
	int timer = 0;
	while (timer < 50)
	{
		GY.kP = 0.4;
		GY.kD = 0.2;
		GY.kI = 0.00008;
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

void ultrasonicPid(double distance)
{
	US.target = distance;
	US.integral = 0;
	int timer = 0;
	while (timer < 100)
	{
		US.kP = 0.1;
		US.kD = 0.05;
		US.kI = 0.00008;
		US.sensor = ultrasonic.get_value();
		US.error = US.target - US.sensor;
		US.derivative = US.error - US.previous_error;
		US.integral += US.error;
		US.previous_error = US.error;
		US.speed = (US.kP * US.error + US.kD * US.derivative + US.kI * US.integral) / 30;

		model->tank(US.speed, US.speed);

		timer++;
		pros::delay(20);
		pros::lcd::set_text(1, "Ultra: " + std::to_string(US.sensor));
	}
	model->tank(0, 0);
}

#define groundPotValue 1200
#define upLittlePotValue 1100
#define platformPotValue 660
#define maxHeightPotValue 130

void liftPid(int potValue)
{
	LIFT.target = potValue;
	LIFT.integral = 0;
	int timer = 0;
	while (timer < 100)
	{
		LIFT.kP = 0.3;
		LIFT.kD = 0.001;
		LIFT.kI = 0.00000;
		LIFT.sensor = liftPot.get();
		LIFT.error = LIFT.target - LIFT.sensor;
		LIFT.derivative = LIFT.error - LIFT.previous_error;
		LIFT.integral += LIFT.error;
		LIFT.previous_error = LIFT.error;
		LIFT.speed = (LIFT.kP * LIFT.error + LIFT.kD * LIFT.derivative + LIFT.kI * LIFT.integral) / 60.0;

		leftLift.moveVelocity(-100 * LIFT.speed);
		rightLift.moveVelocity(-100 * LIFT.speed);
		pros::lcd::set_text(2, "SPEED: " + std::to_string(LIFT.speed * 100));

		timer++;
		pros::delay(20);
		pros::lcd::set_text(1, "LIFT: " + std::to_string(LIFT.sensor));
	}
	leftLift.moveVelocity(0);
	rightLift.moveVelocity(0);
}

void setDriveMotorsToHold()
{
	driveRightBackBack.setBrakeMode(AbstractMotor::brakeMode::hold);
	driveRightBackFront.setBrakeMode(AbstractMotor::brakeMode::hold);
	driveRightFront.setBrakeMode(AbstractMotor::brakeMode::hold);
	driveLeftBackBack.setBrakeMode(AbstractMotor::brakeMode::hold);
	driveLeftBackFront.setBrakeMode(AbstractMotor::brakeMode::hold);
	driveLeftFront.setBrakeMode(AbstractMotor::brakeMode::hold);
}

void setDriveMotorsToCoast()
{
	driveRightBackBack.setBrakeMode(AbstractMotor::brakeMode::coast);
	driveRightBackFront.setBrakeMode(AbstractMotor::brakeMode::coast);
	driveRightFront.setBrakeMode(AbstractMotor::brakeMode::coast);
	driveLeftBackBack.setBrakeMode(AbstractMotor::brakeMode::coast);
	driveLeftBackFront.setBrakeMode(AbstractMotor::brakeMode::coast);
	driveLeftFront.setBrakeMode(AbstractMotor::brakeMode::coast);
}

void driveUpRampUltrasonic()
{
	setDriveMotorsToHold();
	// This function assumes we are set up and just need to move forward until we reach the top of the platform
	// We will use the ultrasonic sensor to do this
	double val = ultrasonic.get_value();
	while (val < 1000)
	{
		val = ultrasonic.get_value();
		model->tank(0.3, 0.3);
		pros::delay(10);
		pros::lcd::clear_line(1);
		pros::lcd::set_text(1, "Ultrasonic: " + std::to_string(val));
	}
	pros::lcd::set_text(2, "Going Fast");
	while (val < 1600)
	{
		val = ultrasonic.get_value();
		model->tank(.4, .4);
		pros::delay(10);
		std::cout << "Ultrasonic: " << val << std::endl;
		pros::lcd::clear_line(1);
		pros::lcd::set_text(1, "Ultrasonic: " + std::to_string(val));
	}
	// model->tank(.35, .35);
	// pros::delay(400);
	model->tank(0, 0);
	// setDriveMotorsToCoast();
}

void driveUpRampGyro()
{
	imu.set_pitch(0.0);
	setDriveMotorsToHold();
	movePID(-5, -5, 1000);
	moveClamp(false);
	// Move slow until the we reach 10 degrees of pitch
	// We will use the gyro to do this
	double pitch = imu.get_pitch();
	while (pitch < 18)
	{
		model->tank(0.25, 0.25);
		pros::delay(20);
		pitch = imu.get_pitch();
	}
	// Now we are on the ramp
	while (pitch > 18)
	{
		model->tank(.35, .35);
		pros::delay(20);
		pitch = imu.get_pitch();
	}
	pros::lcd::set_text(2, "Reached Peak");
	model->tank(-.35, -.35);
	pros::delay(200);
	model->tank(0.0, 0.0);
}

void driveUpRamp()
{
	setDriveMotorsToHold();
	movePID(-4, -4, 1000);
	moveClamp(false);
	driveUpRampUltrasonic();
}

void driveToMiddleAndGrab()
{
	movePID(-54, -41, 1100);
	moveClamp(false);
	movePID(28, 10, 1500);
	double init_rotation = imu.get_rotation();
	gyroPID(init_rotation * -1);
	movePID(30, 30, 1000);
	moveClamp(true);
	ultrasonicPid(158);
	gyroPID(90);
	ultrasonicPid(500);
	driveUpRampGyro();
}

void pickUpGoal()
{
	moveClamp(false);
	liftPid(upLittlePotValue);
}

void placeGoalOnPlatform()
{
	liftPid(platformPotValue);
	movePID(-13, -13, 1000);

	moveClamp(true);
	movePID(13, 13, 1000);
}

void skills()
{
	movePID(-66, -66, 3000);
	pickUpGoal();
	movePID(-5, -5, 1000);
	gyroPID(40);
	movePID(-36, -36, 1000);
	placeGoalOnPlatform();
	gyroPID(90);
	movePID(-35, -35, 3000);
	gyroPID(90);
	movePID(-20, -20, 3000);
	pickUpGoal();
	movePID(-32, -32, 2000);
	gyroPID(90);
	movePID(-35, -35, 2000);
	gyroPID(-90);
	placeGoalOnPlatform();
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
			leftLift.moveVelocity(100);
			rightLift.moveVelocity(100);
		}
		else if (down)
		{
			leftLift.moveVelocity(-100);
			rightLift.moveVelocity(-100);
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
			clamp.moveVelocity(100);
		}
		else if (clampDown)
		{
			clamp.moveVelocity(-100);
		}
		else
		{
			clamp.moveVelocity(0);
		}

		// if (controller.getDigital(ControllerDigital::up))
		// {
		// 	// gyroPID(-90);
		// 	liftPid(upLittlePotValue);
		// }
		// else if (controller.getDigital(ControllerDigital::down))
		// {
		// 	// pros::lcd::clear();
		// 	liftPid(groundPotValue);
		// }
		// else if (controller.getDigital(ControllerDigital::left))
		// {
		// 	// pros::lcd::clear();
		// 	liftPid(platformPotValue);
		// }
		// else if (controller.getDigital(ControllerDigital::right))
		// {
		// 	// pros::lcd::clear();
		// 	liftPid(maxHeightPotValue);
		// }

		// if (controller.getDigital(ControllerDigital::A))
		// {
		// 	driveUpRampGyro();
		// }
		// if (controller.getDigital(ControllerDigital::X))
		// {
		// 	driveToMiddleAndGrab();
		// }
		// if (controller.getDigital(ControllerDigital::Y))
		// {
		// 	skills();
		// }

		// std::cout << model->getSensorVals()[0] << " " << model->getSensorVals()[1] << std::endl;

		pros::delay(20);
		double val = ultrasonic.get_value();
		double heading = imu.get_heading();
		double rotation = imu.get_rotation();

		pros::lcd::set_text(1, "Ultrasonic: " + std::to_string(val));
		pros::lcd::set_text(2, "Heading: " + std::to_string(heading));
		pros::lcd::set_text(3, "Rotation: " + std::to_string(rotation));
		pros::lcd::set_text(5, "Pitch: " + std::to_string(imu.get_pitch()));
		pros::lcd::set_text(6, "Pot: " + std::to_string(liftPot.get()));

		// Print ultrasonic sensor output
		// std::cout << "Ultrasonic: " << ultrasonic.get_value() << std::endl;
	}
}

//Auton Selector
bool selected = false; //TODO: false
int lcdCounter = 0;

void left_button()
{
	if (!selected)
	{
		lcdCounter--;
		if (lcdCounter < 0)
		{
			lcdCounter = 0;
		}
	}
}

void center_button()
{
	selected = true;
}
void right_button()
{
	if (!selected)
	{
		lcdCounter++;
		if (lcdCounter > 6)
		{
			lcdCounter = 6;
		}
	}
}
std::string convert(int arg)
{
	switch (arg)
	{
	case 1:
		return "Middle Fight";
	case 2:
		return "Park on Ramp";
	default:
		return "No Auton";
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
	driveToMiddleAndGrab();
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
	pros::lcd::register_btn0_cb(left_button);
	pros::lcd::register_btn1_cb(center_button);
	pros::lcd::register_btn2_cb(right_button);
	pros::delay(1000);
	clamp.setBrakeMode(AbstractMotor::brakeMode::hold);
	leftLift.setBrakeMode(AbstractMotor::brakeMode::hold);
	rightLift.setBrakeMode(AbstractMotor::brakeMode::hold);
	liftPid(groundPotValue);
	moveClamp(true);
	// TODO:: CHANGE FOR COMP
	// while (!selected)
	// {
	// 	pros::lcd::set_text(1, "Auton: " + convert(lcdCounter));
	// 	pros::delay(20);
	// }
	pros::lcd::set_text(1, "Auton Set: " + convert(lcdCounter));
}