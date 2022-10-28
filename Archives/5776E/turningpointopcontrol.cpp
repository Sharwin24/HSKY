#include "main.h"
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

pid FW;
pid GY;
pid DL;
pid DR;
pid CT; 

int LIGHT_THRESHHOLD = 2500;
bool intakeBall = false;
bool indexerBall = false;
int taskChoice = 0;
int indexerToggle = 0;
int flywheelToggle = 0;

okapi::Motor flywheelTop(2, true, okapi::AbstractMotor::gearset::green);
okapi::Motor flywheelBot(3, false, okapi::AbstractMotor::gearset::green);
okapi::ADIEncoder encoder('C', 'D', true);
okapi::Motor indexer(9, true, okapi::AbstractMotor::gearset::red);
okapi::Motor flipper(5, true, okapi::AbstractMotor::gearset::green);
okapi::ADIGyro gyro1('A', 1);
okapi::ADIGyro gyro2('B', 1);
pros::ADILineSensor intakeLS('H');
pros::ADILineSensor indexerLS('F');
pros::ADIPotentiometer potCollector('E'); 
okapi::Controller controller;
auto chassis = okapi::ChassisControllerFactory::create({1, 12}, {-10, -19}, okapi::AbstractMotor::gearset::green, {4.125, 10});

void flywheelTask(void *param);
void gyroPID(int rotation);
void movePID(double distanceL, double distanceR, int ms);
void flywheelTask2(void *param);
void collectorPID(int deg); 


int lcdCounter = 1;

void opcontrol()
{
	flywheelToggle = 0;
	FW.target = 0;
	while (true)
	{
		//std::cout << intakeLS.get_value() << " " << indexerLS.get_value() << " " << hoodLS.get_value() << " " << intakeBall << " " << indexerBall << " " << hoodBall << std::endl;
		chassis.arcade(controller.getAnalog(ControllerAnalog::leftY), controller.getAnalog(ControllerAnalog::rightX));
		indexer.moveVelocity(200 * controller.getDigital(ControllerDigital::L1) - 200 * controller.getDigital(ControllerDigital::L2));
		flipper.moveVelocity(200 * controller.getDigital(ControllerDigital::up) - 200 * controller.getDigital(ControllerDigital::down));
		
		pros::delay(20);
	}
}

void flywheelTask(void *)
{
	while (true)
	{
		//std::cout << "POS: " << gyro1.getRemapped(359, -359) << std::endl;
		FW.kP = 0.1;
		FW.kD = 0.05;
		FW.kI = 0;
		FW.sensor = encoder.get() * 25;
		//std::cout << "RPM: " << FW.sensor << std::endl;
		encoder.reset();
		FW.error = FW.target - FW.sensor;
		FW.derivative = FW.error - FW.previous_error;
		FW.integral += FW.error;
		FW.previous_error = FW.error;
		FW.speed = FW.kP * FW.error + FW.kD * FW.derivative + FW.kI * FW.integral;

		if (controller.getDigital(ControllerDigital::R2))
		{
			FW.speed = -0.75;
		}
		else if (flywheelToggle == 0)
		{
			FW.speed = 0;
		}
		else if (FW.speed < 0.5)
		{
			FW.speed = 0.5;
		}

		flywheelTop.controllerSet(FW.speed);
		flywheelBot.controllerSet(FW.speed);

		pros::delay(20);
	}
}
void flywheelTask2(void *)
{
	while (true)
	{
		if (controller.getDigital(ControllerDigital::R1))
		{
			pros::delay(20);
			flywheelToggle++;
			if (flywheelToggle > 2)
			{
				flywheelToggle = 0;
			}

			switch (flywheelToggle)
			{
			case 0:
				FW.target = 0;
				break;
			case 1:
				FW.target = 2000;
				break;
			case 2:
				FW.target = 2500;
				break;
			}

			while (controller.getDigital(ControllerDigital::R1))
			{
				pros::delay(20);
			}
			pros::delay(50);
		}
	}
}
void lineTask(void *) {
	while (true) {
		intakeBall = intakeLS.get_value() < LIGHT_THRESHHOLD;
		indexerBall = indexerLS.get_value() < LIGHT_THRESHHOLD;
		//hoodBall = hoodLS.get_value() < LIGHT_THRESHHOLD;

		pros::delay(10);
	}
}
void ballTask(void *) {
	while (true) {
		if (taskChoice == 1) {
			indexer.moveVelocity(0);
			while (!intakeBall) {
				pros::delay(10);
			}
			indexer.moveVelocity(50);
			while (!indexerBall) {
				pros::delay(10);
			}
			indexer.moveVelocity(0);

			taskChoice = 0;
		}

		pros::delay(10);
	}

}
void gyroPID(int rotation)
{
	GY.target = rotation;
	gyro2.reset();
	GY.integral = 0;
	bool val = false;
	int timer = 0;
	while (timer < 50)
	{
		GY.kP = 0.1;
		GY.kD = 0.05;
		GY.kI = 0;
		GY.sensor = gyro2.get();
		//std::cout << "POS: " << GY.sensor << std::endl;
		GY.error = GY.target - GY.sensor;
		GY.derivative = GY.error - GY.previous_error;
		GY.integral += GY.error;
		GY.previous_error = GY.error;
		GY.speed = (GY.kP * GY.error + GY.kD * GY.derivative + GY.kI * GY.integral) * 2.0 / 127;

		chassis.tank(GY.speed, -1 * GY.speed);

		timer++;
		pros::delay(20);
	}
	chassis.tank(0, 0);
}

void collectorPID(int deg)
{
	CT.target = deg;
	CT.integral = 0;
	int timer = 0; 
	while (timer < 50)
	{
		CT.kP = 0.1;
		CT.kD = 0.01;
		CT.kI = 0;
		CT.sensor = potCollector.get_value(); 
		CT.error = CT.target - CT.sensor;
		CT.derivative = CT.error - CT.previous_error;
		CT.integral += CT.error;
		CT.previous_error = CT.error;
		CT.speed = (CT.kP * CT.error + CT.kD * CT.derivative + CT.kI * CT.integral); 
		flipper.moveVelocity(CT.speed); 
		timer++;
		pros::delay(20);
		
	}
	//keeps flipper at constant placement
	for(int i = 0; i < 20; i++)
	{
		flipper.moveVelocity(-1); 
		flipper.moveVelocity(1);
	}
}

void movePID(double distanceL, double distanceR, int ms) {
	double targetL = distanceL * 360 / (2 * 3.1415 * (4.125 / 2));
	double targetR = distanceR * 360 / (2 * 3.1415 * (4.125 / 2));
	auto drivePIDL = okapi::IterativeControllerFactory::posPID(0.00275, 0.001, 0.0015);
	auto drivePIDR = okapi::IterativeControllerFactory::posPID(0.00257, 0.001, 0.0015);

	chassis.resetSensors();

	int timer = 0;
	double errorL;
	double errorR;
	double powerL;
	double powerR;
	while(timer < ms){

		errorL = targetL - chassis.getSensorVals()[0];
		errorR = targetR - chassis.getSensorVals()[1];
		powerL = drivePIDL.step(errorL);
		powerR = drivePIDR.step(errorR);
		chassis.tank(-powerL, -powerR);
		//std::cout << errorL << " " << errorR << std::endl;

		pros::delay(10);
		timer += 10;
	}

	chassis.tank(0, 0);
}

void blueFront();
void redFront();
void blueBack();
void redBack();
void blueBackTrap();
void redBackTrap();

void autonomous()
{
	switch (lcdCounter)
	{
	case 1:
		blueFront();
		break;
	case 2:
		redFront();
		break;
	case 3:
		blueBack();
		break;
	case 4:
		redBack();
		break;
	case 5:
		blueBackTrap();
		break;
	case 6:
		redBackTrap();
		break;
	}
}

void blueFront()
{

	//setup
	FW.target = 2500;
	flywheelToggle = 2;

	//intake ball from under cap
	taskChoice = 1;
	movePID(34, 34, 1400);

	//move to shooting position at blue tile
	movePID(-35, -35, 1500);
	movePID(10.5, -10.5, 1000);
	movePID(-3, -3, 500);

	//shoot balls
	indexer.moveVelocity(200);
	pros::delay(600);

	//hit bottom flag
	movePID(2, -2, 600);
	movePID(34, 34, 1400);
	flipper.moveVelocity(-200);
	pros::delay(300);
	flipper.moveVelocity(200);
	pros::delay(400);
	flipper.moveVelocity(0);
	indexer.moveVelocity(0);

	//flip front cap
	movePID(-27.5, -27.5, 1250);
	movePID(-6.5, 6.5, 600);
	movePID(12.5, 12.5, 1200);

	taskChoice = 1;
	//reach past cap
	collectorPID(1350);
	pros::delay(1000);
	//collect balls
	movePID(-10, -10, 2000);
	//flip up to reset
	flipper.moveVelocity(200);
	pros::delay(300); 
	//flip down below cap
	flipper.moveVelocity(-170); 
	pros::delay(300);
	flipper.moveVelocity(0);
	//move beneath cap
	movePID(11,11,1500);
	//flip cap
	flipper.moveVelocity(200);
	pros::delay(250);
	//adjust for second double shot
	movePID(-2,2,300);
	indexer.moveVelocity(200);
	pros::delay(6000);

	/*
    //setup
    FW.target = 2500;
    flywheelToggle = 2;
    //intake ball from under cap
    taskChoice = 1;
    movePID(34, 34, 1400);
    movePID(-3, -3, 1400);
    movePID(-8.5, 8.5, 800);
    flipper.moveVelocity(-200);
    pros::delay(600);
    flipper.moveVelocity(0);
    movePID(7.5, 7.5, 1000);
    flipper.moveVelocity(200);
    pros::delay(800);
    flipper.moveVelocity(0);
    movePID(20, -20, 2000);
*/

}
void redFront()
{
}

void blueBack() 
{
}
void redBack() 
{
}

void blueBackTrap()
{
}
void redBackTrap()
{
}


/*______________________________________________________________________________________________________________________________________________________________________________*/

bool selected = true;	//TODO: false

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
	if (!selected)
	{
		selected = true;
	}
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
		return "Blue Front";
	case 2:
		return "Red Front";
	case 3:
		return "Blue Back";
	case 4:
		return "Red Back";
	case 5:
		return "Blue Back Trap";
	case 6:
		return "Red Back Trap";
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
void initialize()
{
	pros::lcd::initialize();
	pros::lcd::register_btn0_cb(left_button);
	pros::lcd::register_btn1_cb(center_button);
	pros::lcd::register_btn2_cb(right_button);

	intakeLS.calibrate();
	indexerLS.calibrate();
	//hoodLS.calibrate();

	while (!selected)
	{
		pros::lcd::set_text(0, convert(lcdCounter));
		pros::delay(20);
	}


	pros::lcd::set_text(0, convert(lcdCounter) + " (SELECTED)");

	pros::Task flywheelTaskHandle(flywheelTask);
	pros::Task flywheelTask2Handle(flywheelTask2);
	pros::Task lineTaskHandle(lineTask);
	pros::Task ballTaskHandle(ballTask);
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
void competitionitialize() {}