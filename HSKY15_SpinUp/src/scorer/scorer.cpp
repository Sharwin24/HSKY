#include "scorer.hpp"
#include "okapi/api/control/async/asyncVelIntegratedController.hpp"
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

void initialize() {
    catpultMotor.setBrakeMode(AbstractMotor::brakeMode::brake);
    intakeMotor.setBrakeMode(AbstractMotor::brakeMode::brake);
}

void update() {
    // Override outtake but return to previous intake state
    if (outtakeButton.changedToPressed()) {
        previousIntakeState = currentIntakeState;    // Save intake state from before outtake button was pressed
        currentIntakeState = IntakeState::OUTTAKING; // Outtake while outtake btn is pressed
    } else if (outtakeButton.changedToReleased()) {
        currentIntakeState = previousIntakeState; // Save intake state from before outtake button was pressed
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
    switch (currentIntakeState) {
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