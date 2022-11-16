#include "scorer.hpp"
#include "okapi/impl/device/button/controllerButton.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"

namespace src::Scorer {

// Buttons for controlling the Scorer
ControllerButton intakeToggle(ControllerDigital::R1);
ControllerButton outtakeButton(ControllerDigital::R2);
ControllerButton catapultFire(ControllerDigital::L1);

// Scorer internal state
IntakeState currentIntakeState = IntakeState::STOPPED;
IntakeState previousIntakeState = IntakeState::STOPPED;
CatapultState currentCatapultState = CatapultState::READY;

/**
 * @brief Applies the given IntakeState to the intake motor
 *
 * @param state the desired state of the Intake mechanism
 */
void setIntakeMotion(IntakeState state) {
    currentIntakeState = state;
    switch (state) {
        case IntakeState::STOPPED:
            intakeMotor.moveVoltage(0);
            break;
        case IntakeState::INTAKING:
            intakeMotor.moveVoltage(12000);
            break;
        case IntakeState::OUTTAKING:
            intakeMotor.moveVoltage(-12000);
            break;
    }
}

/**
 * @brief Applies the given CatapultState to the catapult motor
 *
 * @param state the desired state of the Catapult mechanism
 */
void setCatapultMotion(CatapultState state) {
    currentCatapultState = state;
    switch (state) {
        case CatapultState::READY:
            // Do nothing
            break;
        case CatapultState::LAUNCHING:
            // Fire catapult, then reset, and set state to READY
            pullDownAndFireCatapult();
            pros::delay(CATA_PULL_DOWN_DELAY_MS);
            pullDownCatapult();
            currentCatapultState = CatapultState::READY;
            break;
    }
}

/**
 * @brief Pulls down the catapult until the button is pressed
 *
 */
void pullDownCatapult() {
    // Move the motor down until the limit switch is pressed
    while (!catapultButton.get_value()) {
        catapultMotor.moveVoltage(12000);
        pros::delay(10);
    }
    catapultMotor.moveVoltage(0);
}

/**
 * @brief Fires the catapult by moving the motor until the slipgear is engaged
 *
 */
void fireCatapult() {
    catapultMotor.moveRelative(DEGREES_TO_ENGAGE_SLIP_GEAR, 100);
}

/**
 * @brief Pulls down the catapult and fires it with a delay in between
 *
 * @param msDelay the delay in milliseconds between pulling down and firing. Defaults to 100 ms
 */
void pullDownAndFireCatapult(int msDelay) {
    pullDownCatapult();
    pros::delay(msDelay);
    fireCatapult();
}

/**
 * @brief Determines if the 2 given RGB structs are equal. Uses a tolerance to determine equality.
 *
 * @param color1 the first color to compare
 * @param color2 the second color to compare
 * @param tolerance the tolerance to use when comparing the colors
 */
bool rgbEquals(pros::c::optical_rgb_s_t color1, pros::c::optical_rgb_s_t color2, float tolerance = 0.5) {
    return abs(color1.red - color2.red) < tolerance &&
           abs(color1.green - color2.green) < tolerance &&
           abs(color1.blue - color2.blue) < tolerance;
}

/**
 * @brief Rolls the intake until the optical sensor detects the desired color
 *
 * @param desiredColorRGB the desired color to detect
 * @param intakeDirection the direction to roll the intake. Defaults to IntakeState::INTAKING
 */
void rollIntakeUntilColor(pros::c::optical_rgb_s_t desiredColorRGB, IntakeState intakeDirection) {
    // Set the intake to the desired direction
    setIntakeMotion(intakeDirection);

    // Roll the intake until the desired color is detected
    while (!rgbEquals(opticalSensor.get_rgb(), desiredColorRGB)) {
        pros::delay(20);
    }

    // Stop the intake
    setIntakeMotion(IntakeState::STOPPED);
}

/**
 * @brief Rolls the intake until the optical sensor detects red
 *
 * @param intakeDirection the direction to roll the intake. Defaults to IntakeState::INTAKING
 */
void rollIntakeUntilRed(IntakeState intakeDirection) {
    rollIntakeUntilColor({255, 0, 0}, intakeDirection);
}

/**
 * @brief Rolls the intake until the optical sensor detects blue
 *
 * @param intakeDirection the direction to roll the intake. Defaults to IntakeState::INTAKING
 */
void rollIntakeUntilBlue(IntakeState intakeDirection) {
    rollIntakeUntilColor({0, 0, 255}, intakeDirection);
}

/**
 * @brief Polls the Basket Proximity Sensors and returns the number of discs present
 *
 * @return int the number of discs the sensors detect
 */
int getNumDiscsInBasket() {
    // Read each sensor and determine number of discs in basket
    // Basket Sensors should read 0 when no disc is present and 1 when a disc is present
    return bottomBasketSensor.get_value() + middleBasketSensor.get_value() + topBasketSensor.get_value();
}

/**
 * @brief Initializes the Scorer mechanism motors and states
 *
 */
void initialize() {
    catapultMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
    intakeMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
    currentIntakeState = IntakeState::STOPPED;
    currentCatapultState = CatapultState::READY;
}

/**
 * @brief Using Controller input, updates the internal Scorer states
 *
 */
void update() {
    // Override outtake but return to previous intake state
    if (outtakeButton.changedToPressed()) {
        previousIntakeState = currentIntakeState;
        currentIntakeState = IntakeState::OUTTAKING;
    } else if (outtakeButton.changedToReleased()) {
        currentIntakeState = previousIntakeState;
    }

    // Intake Toggle turns Intake on and off
    if (intakeToggle.changedToPressed()) {
        if (currentIntakeState != IntakeState::INTAKING) {
            currentIntakeState = IntakeState::INTAKING;
        } else {
            currentIntakeState = IntakeState::STOPPED;
        }
    }

    // If the catapult is ready, fire it when the fire button is pressed
    // Catapult mechanism will handle FIRING -> READY transition
    if (catapultFire.changedToPressed()) {
        if (currentCatapultState == CatapultState::READY && getNumDiscsInBasket() > 0) {
            currentCatapultState = CatapultState::LAUNCHING;
        }
    }
}

/**
 * @brief Acts on the current internal states of the Scorer Mechanisms
 *
 */
void act() {
    setIntakeMotion(currentIntakeState);
    setCatapultMotion(currentCatapultState);
}

} // namespace src::Scorer