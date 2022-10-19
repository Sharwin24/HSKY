#include "controlvector.hpp"

class MotionProfilePoint {
  public:
    MotionProfilePoint();
    ~MotionProfilePoint();
    ControlVector getControlVector();
    float getLeftVel();
    float getRightVel();
    float getCurve();
    float getTimestamp();

  private:
    ControlVector controlVector;
    float leftVel;
    float rightVel;
    float curve;
    float timestamp;
};