#pragma once

#include "controlvector.hpp"

namespace src::Motion {

/**
 * @brief A class to represent a state along a motion-profiled path as a collection of
 * a ControlVector, left/right chassis velocities, curvature, and timestamp.
 */
class MotionProfilePoint {
  public:
    MotionProfilePoint();
    MotionProfilePoint(ControlVector controlVector, float leftChassisVelocity, float rightChassisVelocity, float curvature, float timestamp);
    ~MotionProfilePoint();
    ControlVector getControlVector() { return this->controlVector; };
    float getLeftChassisVelocity() { return this->leftChassisVelocity; };
    float getRightChassisVelocity() { return this->rightChassisVelocity; };
    float getCurvature() { return this->curvature; };
    float getTimestamp() { return this->timestamp; };

    bool operator==(const MotionProfilePoint &other) const;
    bool operator!=(const MotionProfilePoint &other) const;

  private:
    ControlVector controlVector;
    float leftChassisVelocity;  // [in/s]
    float rightChassisVelocity; // [in/s]
    float curvature;            // [1/in]
    float timestamp;            // [ms]
};

} // namespace src::Motion