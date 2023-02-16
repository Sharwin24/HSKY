#pragma once

#include <memory>

#include "main.h"
#include "motion/motion.hpp"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "okapi/impl/device/controller.hpp"
#include "okapi/impl/device/motor/motorGroup.hpp"
#include "pros/misc.hpp"
#include "robot_constants.hpp"

namespace src::Chassis {

// MotorGroup for the left side of the chassis
static MotorGroup leftChassisMotorGroup = {
    Motor(CHASSIS_PORT_L1, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L3, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L4, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)};

// MotorGroup for the right side of the chassis
static MotorGroup rightChassisMotorGroup = {
    Motor(CHASSIS_PORT_R1, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R2, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R3, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R4, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)};

// Chassis pointer for access to chassis model: chassis->getModel()
extern std::shared_ptr<ChassisController> chassis;
static Controller controller = Controller();

// Control loop functions
extern void initialize();
extern void update();
extern void act();

// Chassis motion functions
extern void movePID(float leftTarget, float rightTarget, int ms, float maxV = 1.0f);
extern void movePIDOdom(float leftTarget, float rightTarget, int ms, float maxV = 1.0f);
extern void gyroPID(float degree, bool CW, int ms = 1000);
extern void turnPID(float degree, bool CW, int ms = 1000, float maxV = 0.5f);
extern void ultrasonicPID(int distance, int ms = 2000);
extern void turnToPoint(float targetX, float targetY);

// Chassis state functions
extern void setRobotStartingPosition(Motion::StartingPosition position);
extern void setChassisBrakeMode(AbstractMotor::brakeMode mode);

// Chassis sensors
static pros::Imu imuSensor = pros::Imu(IMU_PORT);
static pros::ADIUltrasonic ultrasonic(ULTRASONIC_PING_PORT, ULTRASONIC_ECHO_PORT);
static pros::Rotation leftEncoder = pros::Rotation(LEFT_ENCODER_PORT);
static pros::Rotation rightEncoder = pros::Rotation(RIGHT_ENCODER_PORT);
static pros::Rotation horizontalEncoder = pros::Rotation(HORIZONTAL_ENCODER_PORT);

} // namespace src::Chassis