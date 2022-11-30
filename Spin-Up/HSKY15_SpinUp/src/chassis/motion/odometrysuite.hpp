#pragma once
#include "main.h"
#include "pros/rotation.hpp"
#include "robotpose.hpp"

namespace src::Motion {

class OdometrySuite {
  public:
    // Constructor
    OdometrySuite(pros::Rotation l, pros::Rotation r, pros::Rotation h);

    // Control Functions
    void initialize();
    void reset();
    void update();

    // Print Functions
    void printRobotPose();

    // Getters
    RobotPose getRobotPose();

  private:
    // Pointers to Encoders
    pros::Rotation *leftEncoder;
    pros::Rotation *rightEncoder;
    pros::Rotation *horizontalEncoder;

    // Mutex for assigning and accessing RobotPose
    pros::Mutex odometryMutex = pros::Mutex();

    // Pose Variables
    float xPosition;
    float yPosition;
    float orientation;
    // Last Reset Variables
    float leftEncoderAtLastReset = 0;
    float rightEncoderAtLastReset = 0;
    float horizontalEncoderAtLastReset = 0;
    float orientationAtLastReset = 0;
    // Previous Cycle Variables
    float previousLeftEncoderValue;
    float previousRightEncoderValue;
    float previousHorizontalEncoderValue;
    float previousOrientation;
    float previousGlobalX;
    float previousGlobalY;

    // Math Functions
    void cartesian2Polar(float x, float y, float &r, float &theta);
    void polar2Cartesian(float r, float theta, float &x, float &y);
};

// Odometry Task Function
extern void odometryTask(OdometrySuite *odomSuite);

} // namespace src::Motion
