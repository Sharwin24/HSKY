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
CatapultState currentCatapultState = CatapultState::UP;

void setIntakeState(IntakeState state) {
    currentIntakeState = state;
}

void setCatapultState(CatapultState state) {
    currentCatapultState = state;
}

void pullDownCatapult() {
    // Move the motor down until the limit switch is pressed
    while (!catapultButton.get_value()) {
        catpultMotor.moveVelocity(90);
        pros::delay(10);
    }
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

    // Lower Catapult from UP position
    if (catapultFire.changedToPressed()) {
        if (currentCatapultState == CatapultState::UP) {
            currentCatapultState = CatapultState::DOWN; // Lower Catapult
        } else {
            currentCatapultState = CatapultState::UP; // Raise Catapult (Fire)
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
        case CatapultState::UP:
            // Fire Catapult should move motor slightly to engage slipgear
            catpultMotor.moveAbsolute(15, 100);
            break;
        case CatapultState::DOWN:
            pullDownCatapult();
            break;
    }
}

} // namespace src::Scorer