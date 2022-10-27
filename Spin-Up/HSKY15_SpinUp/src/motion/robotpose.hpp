/**
 * @brief A class to represent the current position and orientation of the robot 
 * with x position, y position, and angle.
 */
class RobotPose {
  public:
    RobotPose();
    ~RobotPose();
    float getXPosition();
    float getYPosition();
    float getTheta();

  private:
    float xPosition;
    float yPosition;
    float theta;
};