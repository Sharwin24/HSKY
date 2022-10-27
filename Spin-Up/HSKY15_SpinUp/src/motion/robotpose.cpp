#include "robotpose.hpp"

/**
 * @brief Construct a new Robot Pose object that represents the current position 
 * and orientation of the robot with x position, y position, and angle.
 */
RobotPose::RobotPose() {
    this->xPosition = 0;
    this->yPosition = 0;
    this->theta = 0;
}

/**
 * @brief Destroy the Robot Pose object
 */
RobotPose::~RobotPose(){}

/**
 * @brief Gets the current x-position of the robot.
 * 
 * @return the current x-position of the robot as a float.
 */
float RobotPose::getXPosition() {
    return this->xPosition;
}

/**
 * @brief Gets the current y-position of the robot.
 *
 * @return the current y-position of the robot as a float.
 */
float RobotPose::getYPosition() {
    return this->yPosition;
}

/**
 * @brief Gets the current angle of the robot.
 *
 * @return the current angle of the robot as a float
 */
float RobotPose::getTheta() {
    return this->theta;
}