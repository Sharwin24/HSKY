#include "robotpose.hpp"

class ControlVector {
    public:
        ControlVector();
        ~ControlVector();
        RobotPose getPose();
        float getVel();
        float getAcc();
        float getJerk();

    private:
        RobotPose pose;
        float vel;
        float acc;
        float jerk;
};