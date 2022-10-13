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
ControllerButton indexerButton(ControllerDigital::R1);
ControllerButton flywheelToggle(ControllerDigital::R2);

IntakeState previousIntakeState = IntakeState::STOPPED;
IntakeState currentIntakeState = IntakeState::STOPPED;
FlywheelState currentFlywheelState = FlywheelState::OFF;
FlywheelControlAlgorithm flywheelControlAlgorithm = FlywheelControlAlgorithm::BANG_BANG;
static float_t flywheelTargetRPM = 0;

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
 * @brief Applies the given FlywheelState to the flywheel motorgroup
 *
 * @param flywheelState the desired state of the Flywheel mechanism
 */
void setFlywheelMotion(FlywheelState state) {
    currentFlywheelState = state;
    switch (state) {
        case FlywheelState::OFF:
            flywheelTargetRPM = 0;
            break;
        case FlywheelState::HALF_SPEED:
            flywheelTargetRPM = 2000;
            break;
        case FlywheelState::FULL_SPEED:
            flywheelTargetRPM = 2500;
            break;
    }
}

/**
 * @brief Using the assigned control algorithm, applies the target RPM to the flywheel
 *
 */
void flywheelControlTask(void *) {
    while (true) {
        switch (flywheelControlAlgorithm) {
            case FlywheelControlAlgorithm::PID:
                // Flywheel P Controller
                break;
            case FlywheelControlAlgorithm::BANG_BANG:
                // Bang bang control is a simple on/off control algorithm
                // if the current velocity is less than the target velocity, turn the flywheel on
                // otherwise turn the flywheel off
                if ((flywheelMotorGroup.getActualVelocity() * FLYWHEEL_GEAR_RATIO) < flywheelTargetRPM) {
                    flywheelMotorGroup.moveVoltage(12000);
                } else {
                    flywheelMotorGroup.moveVoltage(0);
                }
                break;
        }
        pros::delay(20);
    }
}

/**
 * @brief Advances the state of the flywheel
 *
 */
void flywheelStateTask(void *) {
    while (true) {
        if (flywheelToggle.changedToPressed()) {
            switch (currentFlywheelState) {
                case FlywheelState::OFF:
                    setFlywheelMotion(FlywheelState::HALF_SPEED);
                    break;
                case FlywheelState::HALF_SPEED:
                    setFlywheelMotion(FlywheelState::FULL_SPEED);
                    break;
                case FlywheelState::FULL_SPEED:
                    setFlywheelMotion(FlywheelState::OFF);
                    break;
            }
        }
        pros::delay(20);
    }
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
    flywheelMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    intakeMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
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
}

void act() {
    // Act on the current intake state
    setIntakeMotion(currentIntakeState);
}

} // namespace src::Scorer