class RobotPose {
  public:
    RobotPose();
    ~RobotPose();
    float getX();
    float getY();
    float getTheta();

  private:
    float x;
    float y;
    float theta;
};