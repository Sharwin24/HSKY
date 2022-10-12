#pragma once

#include "main.h"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/optical.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Scorer {

/**
 * @brief Enum to represent a state of the Intake mechanism
 *
 */
enum class IntakeState {
    STOPPED = 0,
    INTAKING = 1,
    OUTTAKING = 2,
};

/**
 * @brief Enum to represent a state of the Flywheel mechanism
 *
 */
enum class FlywheelState {
    OFF = 0,
    HALF_SPEED = 1,
    FULL_SPEED = 2,
};

// Scorer control loop functions
extern void initialize();
extern void update();
extern void act();

// Scorer mechanism functions
extern void setIntakeMotion(IntakeState state);
extern void setFlywheelMotion(FlywheelState state);
extern void rollIntakeUntilRed(IntakeState intakeDirection = IntakeState::INTAKING);
extern void rollIntakeUntilBlue(IntakeState intakeDirection = IntakeState::INTAKING);

// Scorer motors
static Motor intakeMotor = Motor(INTAKE_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
static Motor indexerMotor = Motor(INDEXER_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
static MotorGroup flywheelMotorGroup = {
    Motor(FLYWHEEL_TOP_PORT, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(FLYWHEEL_BOTTOM_PORT, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
};

// Scorer sensors
static pros::Optical opticalSensor = pros::Optical(OPTICAL_SENSOR_PORT);
// static pros::Rotation flywheelEncoder = pros::Rotation(FLYWHEEL_ENCODER_PORT);

} // namespace src::Scorer