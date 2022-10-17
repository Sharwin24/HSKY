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
 * @brief Enum to represent a state of the Indexer mechanism
 *
 */
enum class IndexerState {
    STOPPED = 0,
    INDEXING = 1,
    OUTDEXING = 2, // Outdexing is a word now
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

/**
 * @brief Enum to represent the control algorithm for the Flywheel
 *
 */
enum class FlywheelControlAlgorithm {
    NONE = 0,
    PID = 1,
    BANG_BANG = 2,
    TAKE_BACK_HALF = 3,
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
extern void flywheelStateTask(void *param);
extern void flywheelControlTask(void *param);

// Scorer motors
static Motor intakeMotor = Motor(INTAKE_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
static Motor indexerMotor = Motor(INDEXER_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
static MotorGroup flywheelMotorGroup = {
    Motor(FLYWHEEL_TOP_PORT, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(FLYWHEEL_BOTTOM_PORT, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
};

// Scorer sensors
static pros::Optical opticalSensor = pros::Optical(OPTICAL_SENSOR_PORT);
static pros::Rotation flywheelEncoder = pros::Rotation(FLYWHEEL_ENCODER_PORT);

} // namespace src::Scorer