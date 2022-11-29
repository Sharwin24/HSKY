#include "fieldconstants.hpp"

namespace src::Motion {
void FieldConstants::setStartingPosition(StartingPosition position) {
    this->startingPosition = position;
    switch (position) { // TODO: Set these constants
        case StartingPosition::RED_FRONT:
            this->setRedGoalPosition(0, 0);
            this->setBlueGoalPosition(0, 0);
            break;
        case StartingPosition::RED_BACK:
            this->setRedGoalPosition(0, 0);
            this->setBlueGoalPosition(0, 0);
            break;
        case StartingPosition::BLUE_FRONT:
            this->setRedGoalPosition(0, 0);
            this->setBlueGoalPosition(0, 0);
            break;
        case StartingPosition::BLUE_BACK:
            this->setRedGoalPosition(0, 0);
            this->setBlueGoalPosition(0, 0);
            break;
    }
}
} // namespace src::Motion