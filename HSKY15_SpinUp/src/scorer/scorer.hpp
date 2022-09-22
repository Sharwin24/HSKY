#pragma once

#include "main.h"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/optical.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Scorer {

enum class IntakeState {
    STOPPED = 0,
    INTAKING = 1,
    OUTTAKING = 2,
};

enum class CatapultState {
    DOWN = 0,
    UP = 1
};

// Control loop functions
extern void initialize();
extern void update();
extern void act();

// Mechanism functions
extern void setIntakeState(IntakeState state);
extern void setCatapultState(CatapultState state);
extern void pullDownCatapult();
extern void rollIntakeUntilColor(pros::c::optical_rgb_s desiredColorRGB, IntakeState intakeDirection = IntakeState::INTAKING);

// Motors
static Motor catpultMotor = Motor(CATAPULT_PORT, false, AbstractMotor::gearset::red, AbstractMotor::encoderUnits::degrees);
static Motor intakeMotor = Motor(INTAKE_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Sensors
static pros::Optical opticalSensor = pros::Optical(OPTICAL_SENSOR_PORT);
static pros::ADIDigitalIn catapultButton = pros::ADIDigitalIn(CATAPULT_BUTTON_PORT);

} // namespace src::Scorer