#pragma once

namespace src::Motion {
/**
 * @brief A class to represent the current position and orientation of the robot
 * with x position, y position, and angle.
 */
class RobotPose {
  public:
    RobotPose();
    RobotPose(float x, float y, float theta);
    ~RobotPose();
    float getXPosition() { return this->xPosition; };
    float getYPosition() { return this->yPosition; };
    float getTheta() { return this->theta; };
    bool operator==(const RobotPose &other) const;
    bool operator!=(const RobotPose &other) const;

    RobotPose normalize();

  private:
    float xPosition; // [in]
    float yPosition; // [in]
    float theta;     // [rad]
};

} // namespace src::Motion