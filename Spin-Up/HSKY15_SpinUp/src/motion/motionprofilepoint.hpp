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
    float getCurve();
    float getTimestamp();

  private:
    ControlVector controlVector;
    float leftChassisVelocity;
    float rightChassisVelocity;
    float curve;
    float timestamp;
};