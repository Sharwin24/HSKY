#include "chassis.hpp"

#include "main.h"
#include "okapi/impl/control/iterative/iterativeControllerFactory.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "pros/misc.h"
#include <cmath>

namespace src::Chassis {

// Initalize chassis pointer using motor group and dimensions
std::shared_ptr<ChassisController> chassis =
    ChassisControllerBuilder()
        .withMotors(leftChassisMotorGroup, rightChassisMotorGroup)
        .withSensors(leftChassisMotorGroup.getEncoder(), rightChassisMotorGroup.getEncoder())
        .withDimensions(AbstractMotor::gearset::blue, {{WHEEL_DIAMETER, WHEEL_TRACK}, imev5BlueTPR})
        .build();

// Field Constants Class
static Motion::FieldConstants fieldConstants = Motion::FieldConstants();

static Motion::OdometrySuite odometrySuite = Motion::OdometrySuite(leftEncoder, rightEncoder, horizontalEncoder);

void setChassisBrakeMode(AbstractMotor::brakeMode mode) {
    chassis->getModel()->setBrakeMode(mode);
}

void resetImu(bool print = true) {
    imuSensor.reset();
    int time = pros::millis();
    int iter = 0;
    while (imuSensor.is_calibrating()) {
        if (print) {
            printf("IMU Calibrating... %d [ms]\n", iter);
        }
        iter += 100;
        if (iter >= 2500) { // IMU should not take more than 2500 ms to calibrate
            if (print) {
                printf("IMU Calibration Failed!\n");
            }
            break;
        }
        pros::delay(100);
    }
    if (print) {
        printf("IMU Calibrated in %d [ms]\n", iter - time);
    }
}

/**
 * @brief Set the Robot's Starting Position as one of the predefined positions on the field
 *
 * @param startPosition an enum representing which position has been selected (based on the autonomous selection)
 */
void setRobotStartingPosition(Motion::StartingPosition startPosition) {
    fieldConstants.setStartingPosition(startPosition);
}

void initialize() {
    setChassisBrakeMode(AbstractMotor::brakeMode::brake);
    resetImu();
    odometrySuite.initialize();
}

void update() {
    // Nifty function that runs in control loop. Useful for debugging during opcontrol
    // printf("IMU: %f", imuSensor.get_heading());
    // printf("Ultrasonic: %d", ultrasonic.get_value());
    // printf("Left Encoder: %d", chassis->getModel()->getSensorVals()[0]);
    // printf("Right Encoder: %d", chassis->getModel()->getSensorVals()[1]);
    odometrySuite.update();
}

void act() { // OpControl for chassis
    chassis->getModel()->arcade(
        controller.getAnalog(ControllerAnalog::leftY),
        controller.getAnalog(ControllerAnalog::rightX) * TURN_FACTOR);
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
    float degreesL = ((leftTarget * 360.0f) / (M_PI * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
    float degreesR = ((rightTarget * 360.0f) / (M_PI * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
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
        timer += 10;
        pros::delay(10);
    }
    chassis->stop();
}

/**
 * @brief Moves the chassis given a target distance for each side [in] within a given time [ms] using a PID control loop.
 * Target distance is relative to the current position. Also takes a maximum velocity to scale the speed of the robot.
 * The robot will exit the control loop after the time limit is reached, regardless if the target is reached or not.
 * Utilizes OdometrySuite Encoders instead of Motor Encoders. Does not reset the encoders.
 *
 * @param leftTarget Target distance for the left side [in]
 * @param rightTarget Target distance for the right side [in]
 * @param ms Time to complete the movement [ms]
 * @param maxV Scalar for velocity of the robot, defaults to 1 [0, 1]
 */
void movePIDOdom(float leftTarget, float rightTarget, int ms, float maxV) {
    float currentLeftTravel = (leftEncoder.get_position() * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;   // [in]
    float currentRightTravel = (rightEncoder.get_position()) * (ENCODER_WHEEL_DIAMETER * M_PI) / 36000.0f; // [in]
    float leftTargetTravel = currentLeftTravel + leftTarget;                                               // [in]
    float rightTargetTravel = currentRightTravel + rightTarget;                                            // [in]
    float prevErrorL = 0;
    float prevErrorR = 0;
    float integralL = 0;
    float integralR = 0;
    int timer = 0;
    while (timer < ms) { // Within time limit, increment PID loop
        // Compute PID values from current wheel travel measurements
        currentLeftTravel = (leftEncoder.get_position() * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;
        currentRightTravel = (rightEncoder.get_position()) * (ENCODER_WHEEL_DIAMETER * M_PI) / 36000.0f;
        float errorL = leftTargetTravel - currentLeftTravel;
        float errorR = rightTargetTravel - currentRightTravel;
        integralL += errorL;
        integralR += errorR;
        float derivativeL = errorL - prevErrorL;
        float derivativeR = errorR - prevErrorR;
        prevErrorL = errorL;
        prevErrorR = errorR;
        // Calculate power using PID
        float powerL = (P_GAIN_DRIVE * errorL) + (I_GAIN_DRIVE * integralL) + (D_GAIN_DRIVE * derivativeL);
        float powerR = (P_GAIN_DRIVE * errorR) + (I_GAIN_DRIVE * integralR) + (D_GAIN_DRIVE * derivativeR);
        chassis->getModel()->tank(powerL * maxV, powerR * maxV);
        timer += 10;
        pros::delay(10);
    }
    chassis->stop();
}

/**
 * @brief Rotates the chassis given a target angle [deg] in the specified direction [CW] within a given time [ms] using a PID control loop .
 * Target angle is relative to the current position. The robot will exit the control loop after the time limit is reached,
 * regardless if the target is reached or not. Utilizes the IMU sensor.
 *
 * @param degree Target angle [deg]
 * @param CW Direction of rotation [true = CW, false = CCW]
 * @param ms Time to complete the movement, defaults to 1000 [ms]
 */
void gyroPID(float degree, bool CW, int ms) {
    float taredRotation = imuSensor.get_heading();
    int timer = 0;
    float prevError = 0;
    float integral = 0;
    while (timer < ms) {
        float sensorVal = imuSensor.get_heading() - taredRotation;
        float error = degree - sensorVal;
        float derivative = error - prevError;
        prevError = error;
        integral += error;
        float power = (P_GAIN_TURN * error) + (I_GAIN_TURN * integral) + (D_GAIN_TURN * derivative);
        if (CW) {
            chassis->getModel()->tank(power, -1.0f * power);
        } else {
            chassis->getModel()->tank(-1.0f * power, power);
        }
        timer += 10;
        pros::delay(10);
    }
    chassis->stop();
}

/**
 * @brief Rotates the chassis given a target angle [deg] in the specified direction [CW] within a given time [ms] using a PID control loop .
 * Target angle is relative to the current position. The robot will exit the control loop after the time limit is reached,
 * regardless if the target is reached or not. Utilizes motor encoders.
 *
 * @param degree Target angle [deg] within [0, 360)
 * @param CW Direction of rotation [true = CW, false = CCW]
 * @param ms Time to complete the movement, defaults to 1000 [ms]
 * @param maxV Scalar for velocity of the robot, defaults to 0.5 [0, 1]
 */
void turnPID(float degree, bool CW, int ms, float maxV) {
    // Arc Length Formula: s = r * theta
    float targetWheelTravel = (WHEEL_TRACK / 2.0f) * degree; // [in]
    if (CW) {
        movePID(targetWheelTravel, -1.0f * targetWheelTravel, ms, maxV);
    } else {
        movePID(-1.0f * targetWheelTravel, targetWheelTravel, ms, maxV);
    }
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
    chassis->stop();
}

/**
 * @brief Using the current RobotPose and a target [X,Y], rotates the chassis to face the target.
 *
 * @param targetX Target X coordinate [in]
 * @param targetY Target Y coordinate [in]
 */
void turnToPoint(float targetX, float targetY) {
    Motion::RobotPose robotPose = odometrySuite.getRobotPose();
    float currentX = robotPose.getXPosition();
    float currentY = robotPose.getYPosition();
    float currentAngle = robotPose.getTheta();
    float deltaX = targetX - currentX;
    float deltaY = targetY - currentY;
    float targetAngle = atan2(deltaY, deltaX) * (180.0f / M_PI);
    float deltaAngle = targetAngle - currentAngle;
    if (deltaAngle > 180.0f) {
        deltaAngle -= 360.0f;
    } else if (deltaAngle < -180.0f) {
        deltaAngle += 360.0f;
    }
    // gyroPID(deltaAngle, deltaAngle > 0);
    turnPID(deltaAngle, deltaAngle > 0);
}

} // namespace src::Chassis
