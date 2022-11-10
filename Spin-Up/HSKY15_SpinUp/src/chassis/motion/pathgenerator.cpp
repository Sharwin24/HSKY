#include "pathgenerator.hpp"
#include "math.h"
#include "vector.hpp"

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
    std::vector<MotionProfilePoint> generatedPath = std::vector<MotionProfilePoint>();
    // Actual Path Generation here (probably)
    return Path(generatedPath);
}

/**
 * @brief Accepts a chronological list of waypoints as controlpoints and injects additional waypoints between them for better resolution.
 * This is step 1 of the path generation process.
 *
 * @param waypoints A chronological list of waypoints to be injected with additional waypoints maintaining the same trajectory
 * @param spacing The desired distance between waypoints [in]
 * @return std::vector<RobotPose>
 */
std::vector<RobotPose> PathGenerator::injectWaypoints(std::vector<RobotPose> waypoints, float spacing) {
    // Spacing = Desired distance between waypoints
    // Find all line segments that compose the given set of waypoints -> (StartPosition, EndPosition)
    std::vector<Vector> lineSegments = std::vector<Vector>();
    for (int i = 0; i < waypoints.size() - 1; i++) {
        lineSegments.push_back(Vector(waypoints[i].getXPosition(), waypoints[i].getYPosition(), waypoints[i + 1].getXPosition(), waypoints[i + 1].getYPosition()));
    }
    // For every line segment:
    // Define a vector as the endPoint - startPoint
    // Find the number of waypoints to be injected -> Math.ceil(vector.magnitude / spacing)
    // Normalize the vector -> vector = vector.normalize() * spacing
    // for i = 0 to numPointsToBeInjected: Add startPoint + (vector * i) to path
    std::vector<RobotPose> injectedWaypoints = std::vector<RobotPose>();
    for (int i = 0; i < lineSegments.size(); i++) {
        Vector v = lineSegments[i];
        int numPointsToBeInjected = ceil(v.getMagnitude() / spacing);
        Vector newVector = v.normalize() * spacing; // Assign newVector to copy of v normalized and scaled by spacing
        for (int j = 0; j < numPointsToBeInjected; j++) {
            Vector newVectorToPoint = Vector(v.getX1(), v.getY1(), v.getX1() + (newVector.getX2() * i), v.getY1() + (newVector.getY2() * i));
            injectedWaypoints.push_back(RobotPose()); // hmm
        }
    }
    // After all line segments, add last point to path

    // This entire method needs to be redone I believe. Injection should be somewhat simple:
    // All LineSegments should be identified and the length should be calculated
    // The number of points that fit on the line segment should be calculated using the spacing
    // The points should be created with the appropriate spacing and position, the orientation should be the same as the line segment
    // This should be done for all line segments
}

/**
 * @brief Accepts a chronological list of waypoints as controlpoints and smooths the path between them creating controlpoints dictating
 * a curve. This is step 2 of the path generation process.
 *
 * @param waypoints
 * @return std::vector<RobotPose>
 */
std::vector<RobotPose> PathGenerator::smoothWaypoints(std::vector<RobotPose> waypoints) {
    // smooth after injection with poses
}

} // namespace src::Motion
