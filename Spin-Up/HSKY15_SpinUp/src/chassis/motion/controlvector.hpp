#pragma once
#include "robotpose.hpp"

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
    float getChassisVelocity();
    float getChassisAcceleration();
    float getChassisJerk();

    bool operator==(const ControlVector &other) const;
    bool operator!=(const ControlVector &other) const;

  private:
    RobotPose pose;
    float chassisVelocity;
    float chassisAcceleration;
    float chassisJerk;
};