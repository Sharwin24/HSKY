#include "pathgenerator.hpp"

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
    // TODO: Implement PathGeneration from Pose waypoints and ControlVector waypoints via waypoint injection/smoothing
}

Path PathGenerator::generatePath(std::vector<ControlVector> waypoints) {
    // TODO: Implement PathGeneration from Pose waypoints and ControlVector waypoints via waypoint injection/smoothing
}

std::vector<RobotPose> PathGenerator::injectWaypoints(std::vector<RobotPose> waypoints) {
    // Spacing = Desired distance between waypoints
    // Find all line segments that compose the given set of waypoints -> (StartPosition, EndPosition)
    // For every line segment:
    // Define a vector as the endPoint - startPoint
    // Find the number of waypoints to be injected -> Math.ceil(vector.magnitude / spacing)
    // Normalize the vector -> vector = vector.normalize() * spacing
    // for i = 0 to numPointsToBeInjected: Add startPoint + (vector * i) to path
    // After all line segments, add last point to path
}

std::vector<ControlVector> PathGenerator::injectWaypoints(std::vector<ControlVector> waypoints) {
    // Inject waypoints from ControlVectors
}

std::vector<RobotPose> PathGenerator::smoothWaypoints(std::vector<RobotPose> waypoints) {
    // smooth after injection with poses
}

std::vector<ControlVector> PathGenerator::smoothWaypoints(std::vector<ControlVector> waypoints) {
    // smooth after injection with vectors
}

} // namespace src::Motion
