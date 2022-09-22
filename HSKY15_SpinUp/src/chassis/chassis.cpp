#include "chassis.hpp"

#include "main.h"
#include "okapi/impl/control/iterative/iterativeControllerFactory.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "pros/misc.h"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Chassis {

std::shared_ptr<ChassisController> chassis =
    ChassisControllerBuilder()
        .withMotors(leftChassisMotorGroup, rightChassisMotorGroup)
        .withDimensions(AbstractMotor::gearset::blue, {{WHEEL_DIAMETER, WHEEL_TRACK}, imev5BlueTPR})
        .build();

pros::Imu imuSensor = pros::Imu(IMU_PORT);
pros::ADIUltrasonic ultrasonic(ULTRASONIC_PING_PORT, ULTRASONIC_ECHO_PORT);

void intialize() {
    leftChassisMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    rightChassisMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    imuSensor.reset();
    pros::delay(1000); // give IMU 1 second to reset
}

void update() {}

void act() {
    Chassis::chassis->getModel()->arcade(
        controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y),
        TURN_FACTOR * controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X));
}

void movePID(float leftTarget, float rightTarget, int ms, float maxV) {
    float degreesL = ((leftTarget * 360) / (pi * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
    float degreesR = ((rightTarget * 360) / (pi * WHEEL_DIAMETER)) * DRIVE_GEAR_RATIO;
    auto drivePIDL = IterativeControllerFactory::posPID(P_GAIN_DRIVE, I_GAIN_DRIVE, D_GAIN_DRIVE);
    auto drivePIDR = IterativeControllerFactory::posPID(P_GAIN_DRIVE, I_GAIN_DRIVE, D_GAIN_DRIVE);
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

void gyroPID(int degree, bool CW, int ms) {
    imuSensor.set_rotation(0);
    int timer = 0;
    float prevError = 0;
    float integral = 0;
    while (timer < (ms / 20)) {
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
        timer++;
        pros::delay(20);
    }
    chassis->getModel()->tank(0, 0);
}

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
