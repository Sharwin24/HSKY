#include "motionprofilepoint.hpp"

ControlVector MotionProfilePoint::getControlVector() {
    return this->controlVector;
}

float MotionProfilePoint::getCurve() {
    return this->curve;
}

float MotionProfilePoint::getLeftVel() {
    return this->leftVel;
}

float MotionProfilePoint::getRightVel() {
    return this->rightVel;
}

float MotionProfilePoint::getTimestamp() {
    return this->timestamp;
}