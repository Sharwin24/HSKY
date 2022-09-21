#include "chassis.hpp"

#include "main.h"
#include "okapi/impl/device/controllerUtil.hpp"
#include "pros/misc.h"
#include "robot_constants.hpp"

namespace src::Chassis {

std::shared_ptr<ChassisController> chassis =
    ChassisControllerBuilder()
        .withMotors(leftChassisMotorGroup, rightChassisMotorGroup)
        .withDimensions(AbstractMotor::gearset::blue, {{WHEEL_DIAMETER, WHEEL_TRACK}, imev5BlueTPR})
        .build();

pros::Imu imuSensor = pros::Imu(IMU_PORT);

void intialize() {
    leftChassisMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    rightChassisMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    imuSensor.reset();
    pros::delay(1000);
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
    // Create PID controllers for each side of the chassis
    auto drivePIDL = okapi::IterativeControllerFactory::posPID(P_GAIN, I_GAIN, D_GAIN);
    auto drivePIDR = okapi::IterativeControllerFactory::posPID(P_GAIN, I_GAIN, D_GAIN);
    // Reset Chassis sensors
    chassis->getModel()->resetSensors();
    // Initialize loop variables
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

} // namespace src::Chassis
