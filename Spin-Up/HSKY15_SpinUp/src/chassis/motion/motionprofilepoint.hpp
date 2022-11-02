#pragma once

#include "controlvector.hpp"

/**
 * @brief A class to represent a state along a motion-profiled path as a collection of
 * a ControlVector, left/right chassis velocities, curvature, and timestamp.
 */
class MotionProfilePoint {
  public:
    MotionProfilePoint();
    ~MotionProfilePoint();
    ControlVector getControlVector();
    float getLeftChassisVelocity();
    float getRightChassisVelocity();
    float getCurvature();
    float getTimestamp();

    bool operator==(const MotionProfilePoint &other) const;
    bool operator!=(const MotionProfilePoint &other) const;

  private:
    ControlVector controlVector;
    float leftChassisVelocity;
    float rightChassisVelocity;
    float curvature;
    float timestamp;
};