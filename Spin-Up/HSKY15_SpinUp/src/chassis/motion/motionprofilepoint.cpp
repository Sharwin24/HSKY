#include "motionprofilepoint.hpp"

namespace src::Motion {
/**
 * @brief Construct a new Motion Profile Point object that represent a state along a
 * motion-profiled path as a collection of a ControlVector, left/right chassis velocities,
 * curvature, and timestamp.
 */
MotionProfilePoint::MotionProfilePoint() {
    this->controlVector = ControlVector();
    this->curvature = 0;
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
 * @brief Gets the curvature at this point.
 *
 * @return the curvature of the point as a float.
 */
float MotionProfilePoint::getCurvature() {
    return this->curvature;
}

/**
 * @brief Gets the velocity of the left side of the chassis.
 *
 * @return the left-side velocity as a float [Motor RPM]
 */
float MotionProfilePoint::getLeftChassisVelocity() {
    return this->leftChassisVelocity;
}

/**
 * @brief Gets the velocity of the right side of the chassis.
 *
 * @return the right-side velocity as a float [Motor RPM]
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

/**
 * @brief Determines if this MotionProfilePoint is equal to the given MotionProfilePoint
 *
 * @param other The MotionProfilePoint to compare to.
 */
bool MotionProfilePoint::operator==(const MotionProfilePoint &other) const {
    return this->controlVector == other.controlVector && this->curvature == other.curvature &&
           this->leftChassisVelocity == other.leftChassisVelocity && this->rightChassisVelocity == other.rightChassisVelocity &&
           this->timestamp == other.timestamp;
}

/**
 * @brief Determines if this MotionProfilePoint is not equal to the given MotionProfilePoint
 *
 * @param other The MotionProfilePoint to compare to.
 */
bool MotionProfilePoint::operator!=(const MotionProfilePoint &other) const { return !(*this == other); }

} // namespace src::Motion