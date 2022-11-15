#pragma once
#include "robotpose.hpp"

namespace src::Motion {

/**
 * @brief A class to represent a robot's pose and
 * velocity, acceleration, and jerk at that pose.
 */
class ControlVector {
  public:
    ControlVector();
    ControlVector(RobotPose pose, float chassisVelocity, float chassisAcceleration, float chassisJerk);
    ~ControlVector();
    RobotPose getPose();
    float getChassisVelocity() { return this->chassisVelocity; };
    float getChassisAcceleration() { return this->chassisAcceleration; };
    float getChassisJerk() { return this->chassisJerk; };

    bool operator==(const ControlVector &other) const;
    bool operator!=(const ControlVector &other) const;

  private:
    RobotPose pose;
    float chassisVelocity;     // [in/s]
    float chassisAcceleration; // [in/s^2]
    float chassisJerk;         // [in/s^3]
};

} // namespace src::Motion