#include "pathgenerator.hpp"

namespace src::Motion {

/**
 * @brief Construct a new Path Generator object using Robot-Specific MotionConstraints and a time step
 *
 * @param constraints Robot-Specific constraints regarding the robot kinematics
 * @param deltaTime the time step between generated MotionProfilePoints in the path [ms]
 */
PathGenerator::PathGenerator(MotionConstraints constraints, float deltaTime) : constraints(constraints), deltaTime(deltaTime) {}
PathGenerator::~PathGenerator() {}

// TODO: Implement PathGeneration from Pose waypoints and ControlVector waypoints via waypoint injection/smoothing
Path PathGenerator::generatePath(std::vector<RobotPose> waypoints) { return Path(); }
Path PathGenerator::generatePath(std::vector<ControlVector> waypoints) { return Path(); }

std::vector<RobotPose> PathGenerator::injectWaypoints(std::vector<RobotPose> waypoints) { return std::vector<RobotPose>(); }
std::vector<ControlVector> PathGenerator::injectWaypoints(std::vector<ControlVector> waypoints) { return std::vector<ControlVector>(); }

std::vector<RobotPose> PathGenerator::smoothWaypoints(std::vector<RobotPose> waypoints) { return std::vector<RobotPose>(); }
std::vector<ControlVector> PathGenerator::smoothWaypoints(std::vector<ControlVector> waypoints) { return std::vector<ControlVector>(); }

} // namespace src::Motion
