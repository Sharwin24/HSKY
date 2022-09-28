#pragma once

#include <memory>

#include "main.h"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "pros/misc.hpp"
#include "robot_constants.hpp"

using namespace okapi;

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

// Chassis pointer for motion functions
extern std::shared_ptr<ChassisController> chassis;
extern pros::Controller controller;

// Control loop functions
extern void initialize();
extern void update();
extern void act();

// Chassis motion functions
extern void movePID(float leftTarget, float rightTarget, int ms, float maxV = 1);
extern void gyroPID(int degree, bool CW, int ms = 1000);
extern void ultrasonicPID(int distance, int ms = 2000);

// Chassis set functions
extern void setChassisBrakeMode(AbstractMotor::brakeMode mode);

// Define chassis related sensors
static pros::Imu imuSensor = pros::Imu(IMU_PORT);
static pros::ADIUltrasonic ultrasonic(ULTRASONIC_PING_PORT, ULTRASONIC_ECHO_PORT);

} // namespace src::Chassis