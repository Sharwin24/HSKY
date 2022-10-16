#include "scorer.hpp"
#include "okapi/impl/device/button/controllerButton.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Scorer {

ControllerButton intakeToggle(ControllerDigital::L1);
ControllerButton outtakeButton(ControllerDigital::L2);
ControllerButton catapultFire(ControllerDigital::R1);

IntakeState currentIntakeState = IntakeState::STOPPED;
IntakeState previousIntakeState = IntakeState::STOPPED;
CatapultState currentCatapultState = CatapultState::READY;

/**
 * @brief Applies the given IntakeState to the intake motor
 *
 * @param intakeState the desired state of the Intake mechanism
 */
void setIntakeMotion(IntakeState intakeState) {
    switch (intakeState) {
        case IntakeState::STOPPED:
            intakeMotor.moveVelocity(0);
            break;
        case IntakeState::INTAKING:
            intakeMotor.moveVelocity(200);
            break;
        case IntakeState::OUTTAKING:
            intakeMotor.moveVelocity(-200);
            break;
    }
}

/**
 * @brief Sets the currentCatapult state. Will override the current state
 *
 * @param state the desired state to set the catapult to
 */
void setCatapultState(CatapultState state) {
    currentCatapultState = state;
}

/**
 * @brief Pulls down the catapult until the button is pressed
 *
 */
void pullDownCatapult() {
    // Move the motor down until the limit switch is pressed
    while (!catapultButton.get_value()) {
        catpultMotor.moveVelocity(100);
        pros::delay(10);
    }
}

/**
 * @brief Fires the catapult by moving the motor until the slipgear is engaged
 *
 */
void fireCatapult() {
    catpultMotor.moveAbsolute(DEGREES_TO_ENGAGE_SLIP_GEAR, 100);
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
    rollIntakeUntilColor(RED_COLOR, intakeDirection);
}

/**
 * @brief Rolls the intake until the optical sensor detects blue
 *
 * @param intakeDirection the direction to roll the intake. Defaults to IntakeState::INTAKING
 */
void rollIntakeUntilBlue(IntakeState intakeDirection) {
    rollIntakeUntilColor(BLUE_COLOR, intakeDirection);
}

void initialize() {
    catpultMotor.setBrakeMode(AbstractMotor::brakeMode::hold);
    intakeMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
}

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
        if (currentCatapultState == CatapultState::READY) {
            currentCatapultState = CatapultState::FIRING;
        }
    }
}

void act() {
    // Act on the current intake state
    setIntakeMotion(currentIntakeState);

    // Act on the current catapult state
    switch (currentCatapultState) {
        case CatapultState::READY:
            // Do nothing
            break;
        case CatapultState::FIRING:
            // Fire catapult, then reset, and set state to READY
            pullDownAndFireCatapult();
            pullDownCatapult();
            currentCatapultState = CatapultState::READY;
            break;
    }
}

} // namespace src::Scorer