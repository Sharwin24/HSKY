#pragma once

#include "motionprofilepoint.hpp"
#include <vector>

namespace src::Motion {

/**
 * @brief A class to represent a list of states along a motion-profiled path in chronological order.
 */
class Path {
  public:
    Path();
    Path(std::vector<MotionProfilePoint> points);
    ~Path();
    std::vector<MotionProfilePoint> getProfilePoints();

    bool operator==(const Path &other) const;
    bool operator!=(const Path &other) const;

  private:
    std::vector<MotionProfilePoint> profilePoints;
};

} // namespace src::Motion