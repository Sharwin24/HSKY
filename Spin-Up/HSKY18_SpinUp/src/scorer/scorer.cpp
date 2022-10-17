#include "scorer.hpp"
#include "okapi/impl/device/button/controllerButton.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Scorer {

// Buttons for controlling the Scorer
ControllerButton intakeToggle(ControllerDigital::R1);
ControllerButton outtakeButton(ControllerDigital::R2);
ControllerButton indexerButton(ControllerDigital::L1);
ControllerButton flywheelToggle(ControllerDigital::L2);

// Scorer internal state
IntakeState previousIntakeState = IntakeState::STOPPED;
IntakeState currentIntakeState = IntakeState::STOPPED;
IndexerState previousIndexerState = IndexerState::STOPPED;
IndexerState currentIndexerState = IndexerState::STOPPED;
FlywheelState currentFlywheelState = FlywheelState::OFF;
FlywheelControlAlgorithm flywheelControlAlgorithm = FlywheelControlAlgorithm::BANG_BANG;
static float_t flywheelTargetRPM = 0;

/**
 * @brief Applies the given IntakeState to the intake motor
 *
 * @param state the desired state of the Intake mechanism
 */
void setIntakeMotion(IntakeState state) {
    switch (state) {
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
 * @brief  Applies the given IndexerState to the intake motor
 *
 * @param state the desired state of the Indexer mechanism
 */
void setIndexerMotion(IndexerState state) {
    switch (state) {
        case IndexerState::STOPPED:
            indexerMotor.moveVelocity(0);
            break;
        case IndexerState::INDEXING:
            indexerMotor.moveVelocity(200);
            break;
        case IndexerState::OUTDEXING:
            indexerMotor.moveVelocity(-200);
            break;
    }
}

/**
 * @brief Applies the given FlywheelState to the flywheel motorgroup
 *
 * @param state the desired state of the Flywheel mechanism
 */
void setFlywheelMotion(FlywheelState state) {
    currentFlywheelState = state;
    switch (state) {
        case FlywheelState::OFF:
            flywheelTargetRPM = 0;
            break;
        case FlywheelState::HALF_SPEED:
            flywheelTargetRPM = 1900;
            break;
        case FlywheelState::FULL_SPEED:
            flywheelTargetRPM = 2300;
            break;
    }
}

/**
 * @brief Using the current control algorithm, applies the target RPM to the flywheel
 *
 */
void flywheelControlTask(void *) {
    while (true) {
        if (currentFlywheelState == FlywheelState::OFF) {
            continue;
        }
        switch (flywheelControlAlgorithm) {
            case FlywheelControlAlgorithm::NONE:
                flywheelMotorGroup.moveVelocity(0);
                break;
            case FlywheelControlAlgorithm::PID:
                // Flywheel PID Controller
                break;
            case FlywheelControlAlgorithm::TAKE_BACK_HALF:
                // Flywheel Take Back Half Controller
                break;
            case FlywheelControlAlgorithm::BANG_BANG:
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
 * @brief Advances the state of the flywheel to the next state in the sequence
 *
 */
void flywheelStateTask(void *) {
    while (true) {
        if (flywheelToggle.changedToPressed()) {
            switch (currentFlywheelState) {
                case FlywheelState::OFF:
                    currentFlywheelState = FlywheelState::HALF_SPEED;
                    break;
                case FlywheelState::HALF_SPEED:
                    currentFlywheelState = FlywheelState::FULL_SPEED;
                    break;
                case FlywheelState::FULL_SPEED:
                    currentFlywheelState = FlywheelState::OFF;
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

/**
 * @brief Initializes the Scorer mechanism motors and states
 *
 */
void initialize() {
    flywheelMotorGroup.setBrakeMode(AbstractMotor::brakeMode::coast);
    intakeMotor.setBrakeMode(AbstractMotor::brakeMode::coast);
    indexerMotor.setBrakeMode(AbstractMotor::brakeMode::brake);
    currentIndexerState = IndexerState::STOPPED;
    currentIntakeState = IntakeState::STOPPED;
    currentFlywheelState = FlywheelState::OFF;
    flywheelControlAlgorithm = FlywheelControlAlgorithm::BANG_BANG;
    flywheelTargetRPM = 0;
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

    // Indexer Toggle turns Indexer on and off
    if (indexerButton.changedToPressed()) {
        if (currentIndexerState != IndexerState::INDEXING) {
            currentIndexerState = IndexerState::INDEXING;
        } else {
            currentIndexerState = IndexerState::STOPPED;
        }
    }
}

/**
 * @brief Acts on the current internal states of the Scorer Mechanisms
 *
 */
void act() {
    // Act on the current scorer state
    setIntakeMotion(currentIntakeState);
    setFlywheelMotion(currentFlywheelState);
    setIndexerMotion(currentIndexerState);
}

} // namespace src::Scorer