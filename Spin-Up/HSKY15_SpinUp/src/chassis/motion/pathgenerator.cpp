#include "pathgenerator.hpp"
#include "controlvector.hpp"
#include "motionprofilepoint.hpp"
#include "robotpose.hpp"
#include "vector.hpp"
#include <cmath>
#include <vector>

namespace src::Motion {

// https://drive.google.com/drive/u/0/folders/1MIEETCykfGkvhkjDeqdS8hlHDgFc1MsG

/**
 * @brief Construct a new Path Generator object using Robot-Specific MotionConstraints and a time step
 *
 * @param constraints Robot-Specific constraints regarding the robot kinematics
 * @param deltaTime the time step between generated MotionProfilePoints in the path [ms]
 */
PathGenerator::PathGenerator(MotionConstraints constraints, float deltaTime) : constraints(constraints), deltaTime(deltaTime) {
    // Do some magic I guess?
    // Something needs to happen here to establish how the constraints and deltaTime are used for injection, smoothing, and generation
}
PathGenerator::~PathGenerator() {}

Path PathGenerator::generatePath(std::vector<RobotPose> waypoints) {
    // Injection and Smoothing
    std::vector<RobotPose> injectedWaypoints = injectWaypoints(waypoints);
    std::vector<RobotPose> smoothedWaypoints = smoothWaypoints(injectedWaypoints);
    // Motion Profiling
    std::vector<MotionProfilePoint> generatedPath = std::vector<MotionProfilePoint>();
    float currentTime = 0.0f;
    for (int wp = 0; wp < smoothedWaypoints.size(); wp++) {
        RobotPose currentPose = smoothedWaypoints[wp];
        RobotPose nextPose = smoothedWaypoints[wp + 1];
        // Create a ControlVector using Pose and Chassis Motion
        // Create a MotionProfilePoint using ControlVector and desired left/right velocities with deltaTime from timestep
    }

    return Path(generatedPath);
}

/**
 * @brief Accepts a chronological list of waypoints as controlpoints and injects additional waypoints between them for better resolution.
 * This is step 1 of the path generation process.
 *
 * @param waypoints A chronological list of waypoints to be injected with additional waypoints maintaining the same trajectory
 * @param spacing The desired distance between waypoints. Defaults to 6" [in]
 * @return std::vector<RobotPose> a new list with the injected waypoints
 */
std::vector<RobotPose> PathGenerator::injectWaypoints(std::vector<RobotPose> waypoints, float spacing) {
    // Compose LineSegments that compose set of waypoints
    std::vector<Vector> lineSegments = std::vector<Vector>();
    for (int i = 0; i < waypoints.size() - 1; i++) {
        lineSegments.push_back(Vector(waypoints[i], waypoints[i + 1]));
    }
    // Inject waypoints between line segments
    std::vector<RobotPose> injectedWaypoints = std::vector<RobotPose>();
    for (int i = 0; i < lineSegments.size(); i++) {
        Vector v = lineSegments[i];
        int numPointsToBeInjected = floor(v.getMagnitude() / spacing);
        injectedWaypoints.push_back(waypoints[i]);         // Add start point of line segment
        for (int p = 1; p <= numPointsToBeInjected; p++) { // Add points between start and end of line segment
            float stepDistance = spacing * p;              // [in]
            float newX = v.getX1() + (stepDistance * cos(v.getAngle()));
            float newY = v.getY1() + (stepDistance * sin(v.getAngle()));
            injectedWaypoints.push_back(RobotPose(newX, newY, v.getAngle()));
        }
    }
    // Add last waypoint
    injectedWaypoints.push_back(waypoints[waypoints.size() - 1]);
    return injectedWaypoints;
}

/**
 * @brief Accepts a chronological list of waypoints as controlpoints and smooths the path between them creating controlpoints dictating
 * a curve. This is step 2 of the path generation process.
 *
 * @param waypoints
 * @return std::vector<RobotPose>
 */
std::vector<RobotPose> PathGenerator::smoothWaypoints(std::vector<RobotPose> waypoints, float smoothingFactor, float tolerance) {
    float smoothingWeight = 1.0f - smoothingFactor; // A and B is smoothingFactor
    std::vector<RobotPose> smoothedWaypoints = std::vector<RobotPose>(waypoints);
    // Honestly this is some bullshit
    float currentDelta = tolerance;
    while (currentDelta >= tolerance) {
        currentDelta = 0.0f;
        for (int i = 0; i < waypoints.size(); i++) {
            float auxX = smoothedWaypoints[i].getXPosition();
            float auxY = smoothedWaypoints[i].getYPosition();
            float newX = smoothedWaypoints[i].getXPosition() + smoothingWeight * (waypoints[i].getXPosition() - smoothedWaypoints[i].getXPosition()) +
                         smoothingFactor * (smoothedWaypoints[i - 1].getXPosition() + smoothedWaypoints[i + 1].getXPosition() - (2.0f * smoothedWaypoints[i].getXPosition()));
            float newY = smoothedWaypoints[i].getYPosition() + smoothingWeight * (waypoints[i].getYPosition() - smoothedWaypoints[i].getYPosition()) +
                         smoothingFactor * (smoothedWaypoints[i - 1].getYPosition() + smoothedWaypoints[i + 1].getYPosition() - (2.0f * smoothedWaypoints[i].getYPosition()));
            smoothedWaypoints[i] = RobotPose(newX, newY, smoothedWaypoints[i].getTheta());
            currentDelta += std::abs(auxX - newX) + std::abs(auxY - newY);
        }
    }
    return smoothedWaypoints;
}

} // namespace src::Motion
