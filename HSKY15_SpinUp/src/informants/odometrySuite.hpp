#pragma once
#include "main.h"
#include "okapi/api/device/motor/abstractMotor.hpp"
#include "okapi/api/device/rotarysensor/continuousRotarySensor.hpp"
#include "okapi/api/device/rotarysensor/rotarySensor.hpp"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "okapi/impl/device/motor/motorGroup.hpp"
#include "okapi/impl/device/rotarysensor/adiEncoder.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Informants {

typedef struct Pose {
    float x;     // [in]
    float y;     // [in]
    float theta; // [rad]
} Pose_t;

class OdometrySuite {
  public:
    // Constructor and Destructor
    OdometrySuite();
    ~OdometrySuite();

    // Control Functions
    void reset();
    void update();

    // Getters
    float getXPosition();
    float getYPosition();
    float getOrientation();

  private:
    // Sensors
    pros::Rotation leftEncoder = pros::Rotation(LEFT_ENCODER_PORT);
    pros::Rotation rightEncoder = pros::Rotation(RIGHT_ENCODER_PORT);
    pros::Rotation horizontalEncoder = pros::Rotation(HORIZONTAL_ENCODER_PORT);
    // Control Variables
    float xPosition;
    float yPosition;
    float orientation;
};

extern void odometryTask(void *);
extern Pose_t getRobotPose();

} // namespace src::Informants
