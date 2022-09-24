#pragma once

#include "main.h"
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
 * @brief Enum to represent a state of the Catapult mechanism
 *
 */
enum class CatapultState {
    READY = 0,
    FIRING = 2,
};

// Control loop functions
extern void initialize();
extern void update();
extern void act();

// Mechanism functions
extern void setIntakeMotion(IntakeState state);
extern void pullDownCatapult();
extern void fireCatapult();
extern void pullDownAndFireCatapult(int msDelay = 100);
extern void rollIntakeUntilColor(pros::c::optical_rgb_s desiredColorRGB, IntakeState intakeDirection = IntakeState::INTAKING);

// Motors
static Motor catpultMotor = Motor(CATAPULT_PORT, false, AbstractMotor::gearset::red, AbstractMotor::encoderUnits::degrees);
static Motor intakeMotor = Motor(INTAKE_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Sensors
static pros::Optical opticalSensor = pros::Optical(OPTICAL_SENSOR_PORT);
static pros::ADIDigitalIn catapultButton = pros::ADIDigitalIn(CATAPULT_BUTTON_PORT);

} // namespace src::Scorer