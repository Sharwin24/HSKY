#include "controlvector.hpp"

namespace src::Motion {
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
 * @brief Construct a new Control Vector object with the given pose and chassis kinematics
 *
 * @param pose the pose of the robot at this point in the path
 * @param chassisVelocity the chassis velocity at this point in the path [in/s]
 * @param chassisAcceleration the chassis acceleration at this point in the path [in/s^2]
 * @param chassisJerk the chassis jerk at this point in the path [in/s^3]
 */
ControlVector::ControlVector(RobotPose pose, float chassisVelocity, float chassisAcceleration, float chassisJerk) {
    this->pose = pose;
    this->chassisVelocity = chassisVelocity;
    this->chassisAcceleration = chassisAcceleration;
    this->chassisJerk = chassisJerk;
}

/**
 * @brief Destroy the Control Vector object.
 */
ControlVector::~ControlVector() {}

/**
 * @brief Gets the robot's current position.
 *
 * @return the robot's current position as a RobotPose.
 */
RobotPose ControlVector::getPose() {
    return this->pose;
}

/**
 * @brief Determines if this ControlVector is equal to the given ControlVector.
 *
 * @param other The ControlVector to compare to
 */
bool ControlVector::operator==(const ControlVector &other) const {
    return this->pose == other.pose && this->chassisVelocity == other.chassisVelocity && this->chassisAcceleration == other.chassisAcceleration && this->chassisJerk == other.chassisJerk;
}

/**
 * @brief Determines if this ControlVector is not equal to the given ControlVector.
 *
 * @param other The ControlVector to compare to
 */
bool ControlVector::operator!=(const ControlVector &other) const { return !(*this == other); }

} // namespace src::Motion
