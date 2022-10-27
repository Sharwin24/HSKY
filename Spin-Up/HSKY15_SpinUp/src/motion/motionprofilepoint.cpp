#include "motionprofilepoint.hpp"

/**
 * @brief Construct a new Motion Profile Point object that represent a state along a 
 * motion-profiled path as a collection of a ControlVector, left/right chassis velocities, 
 * curvature, and timestamp.
 */
MotionProfilePoint::MotionProfilePoint() {
    this->controlVector = ControlVector();
    this->curve = 0;
    this->leftChassisVelocity = 0;
    this->rightChassisVelocity = 0;
    this->timestamp = 0;
}

/**
 * @brief Destroy the Motion Profile Point object.
 */
MotionProfilePoint::~MotionProfilePoint(){};

/**
 * @brief Gets the ControlVector associated with this point.
 * 
 * @return the current pose, velocity, accelerationm, and jerk as a ControlVector.
 */
ControlVector MotionProfilePoint::getControlVector() {
    return this->controlVector;
}

/**
 * @brief Gets the curve at this point.
 * 
 * @return the curve of the point as a float.
 */
float MotionProfilePoint::getCurve() {
    return this->curve;
}

/**
 * @brief Gets the velocity of the left side of the chassis.
 * 
 * @return the left-side velocity as a float.
 */
float MotionProfilePoint::getLeftChassisVelocity() {
    return this->leftChassisVelocity;
}

/**
 * @brief Gets the velocity of the right side of the chassis.
 * 
 * @return the right-side velocity as a float.
 */
float MotionProfilePoint::getRightChassisVelocity() {
    return this->rightChassisVelocity;
}

/**
 * @brief Gets the timestamp associated with this point.
 * 
 * @return the timestamp as a float.
 */
float MotionProfilePoint::getTimestamp() {
    return this->timestamp;
}