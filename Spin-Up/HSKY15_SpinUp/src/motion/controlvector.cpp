#include "controlvector.hpp"

RobotPose ControlVector::getPose() {
    return this->pose;
}

float ControlVector::getAcc() {
    return this->acc;
}

float ControlVector::getJerk() {
    return this->jerk;
}

float ControlVector::getVel() {
    return this->vel;
}