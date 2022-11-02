#pragma once

#include "motionconstraints.hpp"
#include "path.hpp"
#include <vector>

namespace src::Motion {

class PathGenerator {
  public:
    PathGenerator(MotionConstraints constraints, float deltaTime = 100.0f);
    ~PathGenerator();

    Path generatePath(std::vector<RobotPose> waypoints);
    Path generatePath(std::vector<ControlVector> waypoints);

  private:
    MotionConstraints constraints;
    float deltaTime; // [ms]

    std::vector<RobotPose> injectWaypoints(std::vector<RobotPose> waypoints);
    std::vector<ControlVector> injectWaypoints(std::vector<ControlVector> waypoints);

    std::vector<RobotPose> smoothWaypoints(std::vector<RobotPose> waypoints);
    std::vector<ControlVector> smoothWaypoints(std::vector<ControlVector> waypoints);
};

} // namespace src::Motion