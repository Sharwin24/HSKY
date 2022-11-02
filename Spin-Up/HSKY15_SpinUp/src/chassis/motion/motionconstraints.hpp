#pragma once

namespace src::Motion {

/**
 * @brief Defines the motion constraints for a robot following a motion profiled path.
 *
 */
struct MotionConstraints {

    /**
     * @brief Construct a new Motion Constraints object for a path
     *
     * @param maxVelocity The maximum allowable velocity for the robot [in/s]
     * @param maxAcceleration The maximum allowable acceleration for the robot [in/s^2]
     * @param maxJerk The maximum allowable jerk for the robot [in/s^3]
     * @param maxCurvature The maximum allowable change in heading for the robot [rad/s]
     */
    MotionConstraints(float maxVelocity, float maxAcceleration, float maxJerk, float maxCurvature)
        : maxVelocity(maxVelocity),
          maxAcceleration(maxAcceleration),
          maxJerk(maxJerk),
          maxCurvature(maxCurvature),
          minAcceleration(-maxAcceleration){};

    float maxVelocity;     // [in/s]
    float maxAcceleration; // [in/s^2]
    float maxJerk;         // [in/s^3]
    float maxCurvature;    // [rad/s]
    float minAcceleration; // [in/s^2]
};
} // namespace src::Motion