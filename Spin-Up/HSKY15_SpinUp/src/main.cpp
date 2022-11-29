#include "main.h"
#include "api.h"
#include "autons.hpp"
#include "chassis/chassis.hpp"
#include "okapi/api.hpp"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include "scorer/scorer.hpp"

// Define included namespaces and types
#define Chassis src::Chassis
#define Scorer src::Scorer
#define Pose Chassis::Pose_t
#define AutonSelector src::AutonSelector
#define Auton AutonSelector::Auton
#define AutonRoutines src::AutonRoutines
#define StartingPosition src::Motion::StartingPosition

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    Chassis::initialize();
    Scorer::initialize();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
    AutonSelector::initialize();
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    switch (AutonSelector::getSelectedAuton()) {
        case Auton::SKILLS:
            AutonRoutines::skills();
            Chassis::setRobotStartingPosition(StartingPosition::RED_FRONT);
            break;
        case Auton::AUTON_1:
            AutonRoutines::auton1();
            Chassis::setRobotStartingPosition(StartingPosition::RED_FRONT);
            break;
        case Auton::AUTON_2:
            AutonRoutines::auton2();
            Chassis::setRobotStartingPosition(StartingPosition::BLUE_BACK);
            break;
        case Auton::NO_OPERATION:
            Chassis::setRobotStartingPosition(StartingPosition::RED_BACK);
            break;
    }
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol() {
    while (true) {
        // Subsystem update will manipulate internal state from controller input
        Chassis::update();
        Scorer::update();

        // Subsystem act will apply internal state to the robot
        Chassis::act();
        Scorer::act();

        pros::delay(10);
    }
}
