#include "scorer.hpp"
#include "okapi/impl/device/button/controllerButton.hpp"
#include "okapi/impl/device/controllerUtil.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"
#include <cmath>

namespace src::Scorer {

// Buttons for controlling the Scorer
ControllerButton intakeButton(ControllerDigital::L1);
ControllerButton outtakeButton(ControllerDigital::L2);
ControllerButton indexerButton(ControllerDigital::R1);
ControllerButton flywheelToggle(ControllerDigital::R2);

// Scorer internal state
IntakeState previousIntakeState = IntakeState::STOPPED;
IntakeState currentIntakeState = IntakeState::STOPPED;
IndexerState currentIndexerState = IndexerState::STOPPED;
FlywheelState currentFlywheelState = FlywheelState::OFF;
FlywheelControlAlgorithm flywheelControlAlgorithm = FlywheelControlAlgorithm::BANG_BANG;

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
 * @brief  Applies the given IndexerState to the intake motor
 *
 * @param state the desired state of the Indexer mechanism
 */
void setIndexerMotion(IndexerState state) {
    currentIndexerState = state;
    switch (state) {
        case IndexerState::STOPPED:
            indexerMotor.moveVoltage(0);
            break;
        case IndexerState::INDEXING:
            indexerMotor.moveVoltage(12000);
            break;
        case IndexerState::OUTDEXING:
            indexerMotor.moveVoltage(-12000);
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
}

/**
 * @brief A PID controller for the flywheel velocity control
 *
 */
void flywheelPIDControl() {
    float flywheelTargetRPM = static_cast<int>(currentFlywheelState);
    float derivative = 0;
    float integral = 0;
    float previousError = 0;
    while (true) {
        float error = flywheelTargetRPM - (flywheelMotorGroup.getActualVelocity() * FLYWHEEL_GEAR_RATIO);
        derivative = error - previousError;
        integral += error;
        previousError = error;
        float flywheelSpeed = (error * FW_P_GAIN) + (integral * FW_I_GAIN) + (derivative * FW_D_GAIN);
        flywheelMotorGroup.moveVelocity(flywheelSpeed / FLYWHEEL_GEAR_RATIO);
        pros::delay(20);
    }
}

/**
 * @brief A Take-Back-Half control algorithm for flywheel velocity control
 *
 */
void flywheelTBHControl() {
    float drive = 0;
    float driveAtZero = 0;
    float previousError = 0;
    bool firstZeroCross = true;
    while (true) {
        float flywheelTargetRPM = static_cast<int>(currentFlywheelState);
        float error = flywheelTargetRPM - (flywheelMotorGroup.getActualVelocity() * FLYWHEEL_GEAR_RATIO);
        drive += error * FW_TBH_GAIN;
        if (drive > 1) {
            drive = 1;
        } else if (drive < 0) {
            drive = 0;
        }
        if (squiggles::sgn(error) != squiggles::sgn(previousError)) {
            if (firstZeroCross) {
                drive = flywheelTargetRPM * FW_VOLTAGE_CONSTANT;
                firstZeroCross = false;
            } else {
                drive = 0.5f * (drive + driveAtZero);
            }
            driveAtZero = drive;
        }
        previousError = error;

        flywheelMotorGroup.moveVoltage(drive * 12000.0f);
        pros::delay(20);
    }
}

/**
 * @brief A Bang-Bang control algorithm for flywheel velocity control
 */
void flywheelBangBangControl() {
    while (true) {
        float flywheelTargetRPM = static_cast<int>(currentFlywheelState);
        if (flywheelTargetRPM == 0) {
            flywheelMotorGroup.moveVoltage(0);
            continue;
        }
        float error = flywheelTargetRPM - (flywheelMotorGroup.getActualVelocity() * FLYWHEEL_GEAR_RATIO);
        if (error > (flywheelTargetRPM - FW_THRESHOLD_RPM)) {
            flywheelMotorGroup.moveVoltage(12000);
        } else if (error <= -FW_THRESHOLD_RPM) {
            flywheelMotorGroup.moveVoltage(0);
        } else { // Within threshold window -> Use Feedforward and P Controller
            flywheelMotorGroup.moveVoltage((flywheelTargetRPM * FW_VOLTAGE_CONSTANT) + (error * FW_PROPORTIONAL_GAIN));
        }
    }
}

/**
 * @brief Using the current control algorithm, applies the target RPM to the flywheel.
 * The algorithm cannot change after this task is started
 *
 */
void flywheelControlTask(void *) {
    switch (flywheelControlAlgorithm) {
        case FlywheelControlAlgorithm::NONE:
            // If no control algorithm is selected, the flywheel cannot operate
            break;
        case FlywheelControlAlgorithm::PID:
            flywheelPIDControl();
            break;
        case FlywheelControlAlgorithm::TAKE_BACK_HALF:
            flywheelTBHControl();
            break;
        case FlywheelControlAlgorithm::BANG_BANG:
            flywheelBangBangControl();
            break;
    }
}

/**
 * @brief Advances the state of the flywheel to the next state in the sequence and loops around
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
    // Initialize Scorer Tasks
    pros::Task flywheelControlHandle(flywheelControlTask);
    pros::Task flywheelStateHandle(flywheelStateTask);
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

    // Intake runs while button is being held
    if (intakeButton.changedToPressed()) {
        currentIntakeState = IntakeState::INTAKING;
    } else if (intakeButton.changedToReleased()) {
        currentIntakeState = IntakeState::STOPPED;
    }

    // Indexer runs while button is being held
    if (indexerButton.changedToPressed()) {
        currentIndexerState = IndexerState::INDEXING;
    } else if (indexerButton.changedToReleased()) {
        currentIndexerState = IndexerState::STOPPED;
    }
}

/**
 * @brief Acts on the current internal states of the Scorer Mechanisms
 *
 */
void act() {
    setFlywheelMotion(currentFlywheelState);
    setIntakeMotion(currentIntakeState);
    setIndexerMotion(currentIndexerState);
}

} // namespace src::Scorer