#include "controlvector.hpp"

/**
 * @brief Construct a new Control Vector object to represent a robot's pose and 
 * velocity, acceleration, and jerk at that pose.
 */
ControlVector::ControlVector() {
    this->pose = RobotPose();
    this->chassisAcceleration = 0;
    this->chassisVelocity = 0;
    this->chassisJerk = 0;
}

/**
 * @brief Destroy the Control Vector object.
 */
ControlVector::~ControlVector(){}

/**
 * @brief Gets the robot's current position.
 * 
 * @return the robot's current position as a RobotPose.
 */
RobotPose ControlVector::getPose() {
    return this->pose;
}

/**
 * @brief Gets the robot's current acceleration.
 * 
 * @return the robot's current acceleration as a float.
 */
float ControlVector::getChassisAcceleration() {
    return this->chassisAcceleration;
}

/**
 * @brief Gets the robot's current jerk.
 * 
 * @return the robot's current jerk as a float.
 */
float ControlVector::getChassisJerk() {
    return this->chassisJerk;
}

/**
 * @brief Gets the robot's current velocity.
 *
 * @return the robot's current velocity as a float.
 */
float ControlVector::getChassisVelocity() {
    return this->chassisVelocity;
}