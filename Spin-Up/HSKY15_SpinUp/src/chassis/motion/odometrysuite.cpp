#include "odometrysuite.hpp"
#include "pros/rtos.hpp"
#include "robot_constants.hpp"

#define CTLE CENTER_TO_LEFT_ENCODER
#define CTRE CENTER_TO_RIGHT_ENCODER
#define CTHE CENTER_TO_HORIZONTAL_ENCODER

namespace src::Motion {

OdometrySuite::OdometrySuite(pros::Rotation l, pros::Rotation r, pros::Rotation h) {
    this->leftEncoder = &l;
    this->rightEncoder = &r;
    this->horizontalEncoder = &h;
    this->reset();
}

RobotPose OdometrySuite::getRobotPose() {
    this->odometryMutex.take();
    const RobotPose pose = RobotPose(this->xPosition, this->yPosition, this->orientation);
    this->odometryMutex.give();
    return pose;
}

void odometryTask(OdometrySuite *odomSuite) {
    while (true) {
        odomSuite->update();
        odomSuite->printRobotPose();
        pros::delay(20);
    }
}

void OdometrySuite::initialize() {
    pros::Task odometryHandle(odometryTask, this);
}

void OdometrySuite::printRobotPose() {
    this->odometryMutex.take();
    printf("Odometry -> [X: %f, Y: %f, Theta: %f]\n", this->xPosition, this->yPosition, this->orientation);
    this->odometryMutex.give();
}

void OdometrySuite::reset() {
    this->odometryMutex.take();
    this->xPosition = 0;
    this->yPosition = 0;
    this->orientation = 0;
    this->odometryMutex.give();
    // imuSensor.reset();
    this->leftEncoder->reset();
    this->rightEncoder->reset();
    this->horizontalEncoder->reset();
    this->leftEncoderAtLastReset = 0;
    this->rightEncoderAtLastReset = 0;
    this->horizontalEncoderAtLastReset = 0;
    this->orientationAtLastReset = 0;
    this->previousLeftEncoderValue = 0;
    this->previousRightEncoderValue = 0;
    this->previousHorizontalEncoderValue = 0;
}

void OdometrySuite::update() {
    // Get encoder values and store locally as centidegrees
    float leftEncoderValue = this->leftEncoder->get_position();
    float rightEncoderValue = this->rightEncoder->get_position();
    float horizontalEncoderValue = this->horizontalEncoder->get_position();
    // Calculate the change in encoder values since last cycle and convert to wheel travel
    float leftEncoderDelta = leftEncoderValue - this->previousLeftEncoderValue;
    float rightEncoderDelta = rightEncoderValue - this->previousRightEncoderValue;
    float horizontalEncoderDelta = horizontalEncoderValue - this->previousHorizontalEncoderValue;
    float leftDeltaTravel = (leftEncoderDelta * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;             // [in]
    float rightDeltaTravel = (rightEncoderDelta * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;           // [in]
    float horizontalDeltaTravel = (horizontalEncoderDelta * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f; // [in]
    // Calculate the total change in left and right encoders since last reset and convert to wheel travel
    float leftEncoderTotal = leftEncoderValue - this->leftEncoderAtLastReset;
    float rightEncoderTotal = rightEncoderValue - this->rightEncoderAtLastReset;
    float horizontalEncoderTotal = horizontalEncoderValue - this->horizontalEncoderAtLastReset;
    float totalLeftTravel = (leftEncoderTotal * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;             // [in]
    float totalRightTravel = (rightEncoderTotal * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f;           // [in]
    float totalHorizontalTravel = (horizontalEncoderTotal * (ENCODER_WHEEL_DIAMETER * M_PI)) / 36000.0f; // [in]
    // Calculate new absolute orientation -> oriAtLastReset + (totalRightTravel - totalLeftTravel) / (CTLE + CTRE)
    float absOrientation = this->orientationAtLastReset + (totalRightTravel - totalLeftTravel) / (CTLE + CTRE);
    // Calculate the change in orientation (deltaTheta) since last cycle -> absoluteOrientation - oriAtLastReset
    float deltaTheta = absOrientation - this->previousOrientation;
    float x = 0;
    float y = 0;
    if (deltaTheta == 0) { // leftTravel == rightTravel -> [x, y] = [horizontalDeltaTravel, rightDeltaTravel]
        x = horizontalDeltaTravel;
        y = rightDeltaTravel;
    } else { // [x, y] = 2sin(theta/2) * [(horizontalDeltaTravel/deltaTheta) + CTHE, (rightDeltaTravel/deltaTheta) + CTRE]
        x = 2.0f * sin(absOrientation / 2.0f) * ((horizontalDeltaTravel / deltaTheta) + CTHE);
        y = 2.0f * sin(absOrientation / 2.0f) * ((rightDeltaTravel / deltaTheta) + CTRE);
    }
    // Calculate the average orientation -> (previousOrientation + (deltaTheta / 2))
    float averageOrientation = this->previousOrientation + (deltaTheta / 2.0f);
    // Calculate global offset (deltaD) as local offset [x,y] rotated by -averageOrientation
    // Convert existing cartesian coordinates to polar coordinates, changing the angle, then converting back
    // Calculate new absolute position -> previousGlobalPosition + deltaD
    float localOffsetRadius;
    float localOffsetAngle;
    cartesian2Polar(x, y, localOffsetRadius, localOffsetAngle);
    float globalOffsetX;
    float globalOffsetY;
    polar2Cartesian(localOffsetRadius, localOffsetAngle - averageOrientation, globalOffsetX, globalOffsetY);
    float absX = this->previousGlobalX + globalOffsetX;
    float absY = this->previousGlobalY + globalOffsetY;
    // Assign Final Robot Pose
    this->odometryMutex.take();
    this->xPosition = absX;
    this->yPosition = absY;
    this->orientation = absOrientation;
    this->odometryMutex.give();
    // Update previous values for next cycle
    this->previousLeftEncoderValue = leftEncoderValue;
    this->previousRightEncoderValue = rightEncoderValue;
    this->previousHorizontalEncoderValue = horizontalEncoderValue;
    this->previousGlobalX = absX;
    this->previousGlobalY = absY;
    this->previousOrientation = absOrientation;
}

/**
 * @brief Converts cartesian coordinates to polar coordinates
 *
 * @param x Cartesian x coordinate [in]
 * @param y Cartesian y coordinate [in]
 * @param r Polar radius [in]
 * @param theta Polar angle [rad]
 */
void OdometrySuite::cartesian2Polar(float x, float y, float &r, float &theta) {
    r = sqrt(pow(x, 2) + pow(y, 2)); // [in]
    theta = atan2(y, x);             // [rad]
}

/**
 * @brief Converts polar coordinates to cartesian coordinates
 *
 * @param r Polar radius [in]
 * @param theta Polar angle [rad]
 * @param x Cartesian x coordinate [in]
 * @param y Cartesian y coordinate [in]
 */
void OdometrySuite::polar2Cartesian(float r, float theta, float &x, float &y) {
    x = r * cos(theta); // [in]
    y = r * sin(theta); // [in]
}

} // namespace src::Motion
