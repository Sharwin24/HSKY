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

    std::vector<RobotPose> injectWaypoints(std::vector<RobotPose> waypoints, float spacing);
    std::vector<RobotPose> smoothWaypoints(std::vector<RobotPose> waypoints);
};

} // namespace src::Motion