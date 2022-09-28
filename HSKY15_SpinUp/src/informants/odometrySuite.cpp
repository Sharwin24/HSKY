#pragma once
#include "odometrySuite.hpp"
#include "okapi/api/device/motor/abstractMotor.hpp"
#include "okapi/api/device/rotarysensor/continuousRotarySensor.hpp"
#include "okapi/api/device/rotarysensor/rotarySensor.hpp"
#include "okapi/api/odometry/odometry.hpp"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/device/motor/motor.hpp"
#include "okapi/impl/device/motor/motorGroup.hpp"
#include "okapi/impl/device/rotarysensor/adiEncoder.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"

#define CTLE CENTER_TO_LEFT_ENCODER
#define CTRE CENTER_TO_RIGHT_ENCODER
#define CTHE CENTER_TO_HORIZONTAL_ENCODER

namespace src::Informants {

// Obtainable pose that updates every 10ms
static Pose_t robotPose = {0, 0, 0};

Pose_t getRobotPose() { return robotPose; }

void odometryTask(void *) {
    OdometrySuite odometrySuite = OdometrySuite();
    odometrySuite.reset();
    while (true) {
        odometrySuite.update();
        robotPose.x = odometrySuite.getXPosition();
        robotPose.y = odometrySuite.getYPosition();
        robotPose.theta = odometrySuite.getOrientation();
        pros::delay(10);
    }
}

void OdometrySuite::reset() {
    this->xPosition = 0;
    this->yPosition = 0;
    this->orientation = 0;
    this->leftEncoder.reset();
    this->rightEncoder.reset();
    this->horizontalEncoder.reset();
}

void OdometrySuite::update() {
    float leftTravel = (this->leftEncoder.get_position() * 36000.0f) / (pi * ENCODER_WHEEL_DIAMETER);             // [in]
    float rightTravel = (this->rightEncoder.get_position() * 36000.0f) / (pi * ENCODER_WHEEL_DIAMETER);           // [in]
    float horizontalTravel = (this->horizontalEncoder.get_position() * 36000.0f) / (pi * ENCODER_WHEEL_DIAMETER); // [in]
    this->orientation = (leftTravel - rightTravel) / (CTLE + CTRE);
    this->xPosition = 2.0f * ((horizontalTravel / this->orientation) + CTHE) * sin(this->orientation / 2.0f);
    this->yPosition = 2.0f * ((rightTravel / this->orientation) + CTRE) * sin(this->orientation / 2.0f);
}

float OdometrySuite::getXPosition() { return this->xPosition; }
float OdometrySuite::getYPosition() { return this->yPosition; }
float OdometrySuite::getOrientation() { return this->orientation; }
} // namespace src::Informants
