#include "autons.hpp"
#include "chassis/chassis.hpp"
#include "main.h"
#include "scorer/scorer.hpp"

#define Chassis src::Chassis
#define Scorer src::Scorer

namespace src::AutonRoutines {

void skills() {
    // Scorer::setIntakeMotion(Scorer::IntakeState::INTAKING); // Start intaking
    // Chassis::movePID(24, 24, 1000);                         // move forward 24 inches
    // Chassis::gyroPID(90, true);                             // turn 90 degrees clockwise
    // Scorer::pullDownAndFireCatapult();                      // pull down and fire catapult
    // Scorer::setIntakeMotion(Scorer::IntakeState::STOPPED);  // stop intaking
}

void auton1() {}
void auton2() {}

} // namespace src::AutonRoutines