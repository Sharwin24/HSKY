#include "chassis.hpp"

#include "main.h"
#include "okapi/impl/control/iterative/iterativeControllerFactory.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "pros/misc.h"
#include "robot_constants.hpp"

using namespace okapi;

#define CTLE CENTER_TO_LEFT_ENCODER
#define CTRE CENTER_TO_RIGHT_ENCODER
#define CTHE CENTER_TO_HORIZONTAL_ENCODER

namespace src::Chassis {

// Initalize chassis pointer using motor group and dimensions
std::shared_ptr<ChassisController> chassis =
    ChassisControllerBuilder()
        .withMotors(leftChassisMotorGroup, rightChassisMotorGroup)
        .withSensors(leftChassisMotorGroup.getEncoder(), rightChassisMotorGroup.getEncoder())
        .withDimensions(AbstractMotor::gearset::blue, {{WHEEL_DIAMETER, WHEEL_TRACK}, imev5BlueTPR})
        .build();

// Robot's Pose that is updated by the odometry task every 10 ms
static Pose_t robotPose = {0, 0, 0};

Pose_t getRobotPose() { return robotPose; }

void odometryTask(void *) {
    OdometrySuite odometrySuite = OdometrySuite();
    odometrySuite.reset();
    while (true) {
        odometrySuite.update();
        robotPose.x = odometrySuite.getXPosition();
        robotPose.y = odometrySuite.getYPosition();
        robotPose.theta = odometrySuite.getOrientation();
        pros::delay(10);
    }
}

void OdometrySuite::reset() {
    this->xPosition = 0;
    this->yPosition = 0;
    this->orientation = 0;
    leftEncoder.reset();
    rightEncoder.reset();
    horizontalEncoder.reset();
}

void OdometrySuite::update() {
    float leftTravel = (leftEncoder.get_position() * (pi * ENCODER_WHEEL_DIAMETER)) / 36000.0f;             // [in]
    float rightTravel = (rightEncoder.get_position() * (pi * ENCODER_WHEEL_DIAMETER)) / 36000.0f;           // [in]
    float horizontalTravel = (horizontalEncoder.get_position() * (pi * ENCODER_WHEEL_DIAMETER)) / 36000.0f; // [in]
    this->orientation = (leftTravel - rightTravel) / (CTLE + CTRE);                                         // [rad]
    this->xPosition = 2.0f * ((horizontalTravel / this->orientation) + CTHE) * sin(this->orientation / 2.0f);
    this->yPosition = 2.0f * ((rightTravel / this->orientation) + CTRE) * sin(this->orientation / 2.0f);
}

float OdometrySuite::getXPosition() { return this->xPosition; }
float OdometrySuite::getYPosition() { return this->yPosition; }
float OdometrySuite::getOrientation() { return this->orientation; }

void setChassisBrakeMode(AbstractMotor::brakeMode mode) {
    leftChassisMotorGroup.setBrakeMode(mode);
    rightChassisMotorGroup.setBrakeMode(mode);
}

void initialize() {
    setChassisBrakeMode(AbstractMotor::brakeMode::hold);
    imuSensor.reset();
    int time = pros::millis();
    int iter = 0;
    while (imuSensor.is_calibrating()) {
        printf("IMU Calibrating... %d [ms]\n", iter);
        iter += 100;
        pros::delay(100);
    }
    printf("IMU Calibrated in %d [ms]\n", iter - time);
}

void update() {
    // Print sensor data
    // printf("IMU: %f", imuSensor.get_heading());
    // printf("Ultrasonic: %d", ultrasonic.get_value());
    // printf("Left Encoder: %d", chassis->getModel()->getSensorVals()[0]);
    // printf("Right Encoder: %d", chassis->getModel()->getSensorVals()[1]);
    // printf("RobotPose [%f, %f, %f]", robotPose.x, robotPose.y, robotPose.theta);
}

void act() { // OpControl for chassis
    Chassis::chassis->getModel()->arcade(
        controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y),
        TURN_FACTOR * controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X));
}

/**
 * @brief Moves the chassis given a target distance for each side [in] within a given time [ms] using a PID control loop.
 * Target distance is relative to the current position. Also takes a maximum velocity to scale the speed of the robot.
 * The robot will exit the control loop after the time limit is reached, regardless if the target is reached or not.
 *
 * @param leftTarget Target distance for the left side [in]
 * @param rightTarget Target distance for the right side [in]
 * @param ms Time to complete the movement [ms]
 * @param maxV Scalar for velocity of the robot, defaults to 1 [0, 1]
 */
void movePID(float leftTarget, float rightTarget, int ms, float maxV) {
    float degreesL = ((leftTarget * 360) / (pi * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
    float degreesR = ((rightTarget * 360) / (pi * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
    IterativePosPIDController drivePIDL = IterativeControllerFactory::posPID(P_GAIN_DRIVE, I_GAIN_DRIVE, D_GAIN_DRIVE);
    IterativePosPIDController drivePIDR = IterativeControllerFactory::posPID(P_GAIN_DRIVE, I_GAIN_DRIVE, D_GAIN_DRIVE);
    chassis->getModel()->resetSensors();
    int timer = 0;
    float errorL;
    float errorR;
    float powerL;
    float powerR;
    while (timer < ms) { // Within time limit, increment PID loop
        errorL = degreesL + chassis->getModel()->getSensorVals()[0];
        errorR = degreesR + chassis->getModel()->getSensorVals()[1];
        powerL = drivePIDL.step(errorL);
        powerR = drivePIDR.step(errorR);
        chassis->getModel()->tank(powerL * maxV, powerR * maxV);
        pros::delay(10);
        timer += 10;
    }
    chassis->getModel()->tank(0, 0);
}

/**
 * @brief Rotates the chassis given a target angle [deg] in the specified direction [CW] within a given time [ms] using a PID control loop .
 * Target angle is relative to the current position. The robot will exit the control loop after the time limit is reached,
 * regardless if the target is reached or not.
 *
 * @param degree Target angle [deg]
 * @param CW Direction of rotation [true = CW, false = CCW]
 * @param ms Time to complete the movement, defaults to 1000 [ms]
 */
void gyroPID(int degree, bool CW, int ms) {
    imuSensor.set_rotation(0);
    int timer = 0;
    float prevError = 0;
    float integral = 0;
    while (timer < ms) {
        float sensorVal = imuSensor.get_rotation();
        float error = degree - sensorVal;
        float derivative = error - prevError;
        prevError = error;
        integral += error;
        float power = (P_GAIN_TURN * error) + (I_GAIN_TURN * integral) + (D_GAIN_TURN * derivative);
        if (CW) {
            chassis->getModel()->tank(power, -1 * power);
        } else {
            chassis->getModel()->tank(-1 * power, power);
        }
        timer += 20;
        pros::delay(20);
    }
    chassis->getModel()->tank(0, 0);
}

/**
 * @brief Moves the chassis until the given distance [in] is read by the ultrasonic sensor within a given time [ms] using a PID control loop.
 * Uses less aggressive PID constants to move the robot slower.
 *
 * @param distance Target distance [in]
 * @param ms Time to complete the movement, defaults to 2000 [ms]
 */
void ultrasonicPID(float distance, int ms) {
    float prevError;
    float integral;
    int timer = 0;
    while (timer < ms) {
        float sensorVal = ultrasonic.get_value();
        float error = distance - sensorVal;
        float derivative = error - prevError;
        prevError = error;
        integral += error;
        float power = (P_GAIN_DRIVE_ULTRASONIC * error) + (I_GAIN_DRIVE_ULTRASONIC * integral) + (D_GAIN_DRIVE_ULTRASONIC * derivative);
        chassis->getModel()->tank(power, power);
        timer += 20;
        pros::delay(20);
    }
    chassis->getModel()->tank(0, 0);
}

} // namespace src::Chassis
