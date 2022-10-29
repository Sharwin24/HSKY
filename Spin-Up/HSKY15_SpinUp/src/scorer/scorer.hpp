#pragma once

#include "main.h"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/optical.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Scorer {

// Class for handling suite of proximity sensors within the Scorer mechanism and observing the scorer's state
class ProximitySensorSuite {
  public:
    ProximitySensorSuite(const char bottomPort, const char middlePort, const char topPort);
    ~ProximitySensorSuite();

    int getNumDiscsInBasket();

  private:
    pros::ADIDigitalIn bottomBasketSensor;
    pros::ADIDigitalIn middleBasketSensor;
    pros::ADIDigitalIn topBasketSensor;
    // pros::ADIDigitalIn intakeSensor;
};

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
    FIRING = 1,
};

// Scorer control loop functions
extern void initialize();
extern void update();
extern void act();

// Scorer mechanism functions
extern void setIntakeMotion(IntakeState state);
extern void pullDownCatapult();
extern void fireCatapult();
extern void pullDownAndFireCatapult(int msDelay = 100);
extern void rollIntakeUntilRed(IntakeState intakeDirection = IntakeState::INTAKING);
extern void rollIntakeUntilBlue(IntakeState intakeDirection = IntakeState::INTAKING);

// Scorer motors
static Motor catapultMotor = Motor(CATAPULT_PORT, false, AbstractMotor::gearset::red, AbstractMotor::encoderUnits::degrees);
static Motor intakeMotor = Motor(INTAKE_PORT, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Scorer sensors
static pros::Optical opticalSensor = pros::Optical(OPTICAL_SENSOR_PORT);
static pros::ADIDigitalIn catapultButton = pros::ADIDigitalIn(CATAPULT_BUTTON_PORT);

} // namespace src::Scorer