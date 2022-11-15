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

  private:
    MotionConstraints constraints;
    float deltaTime; // [ms]

    std::vector<RobotPose> injectWaypoints(std::vector<RobotPose> waypoints, float spacing = 6.0f);
    std::vector<RobotPose> smoothWaypoints(std::vector<RobotPose> waypoints, float smooothingFactor = 0.75f, float tolerance = 0.001f);
};

} // namespace src::Motion