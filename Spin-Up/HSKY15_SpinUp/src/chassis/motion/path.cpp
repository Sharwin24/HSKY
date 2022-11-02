#include "path.hpp"

namespace src::Motion {

/**
 * @brief Construct a new Path object that represents a list of states along a motion-profiled path
 * in chronological order.
 */
Path::Path() {
    this->profilePoints = std::vector<MotionProfilePoint>();
}

/**
 * @brief Construct a new Path with the given list of MotionProfilePoints.
 *
 * @param points a list of MotionProfilePoints in chronological order.
 */
Path::Path(std::vector<MotionProfilePoint> points) {
    this->profilePoints = points;
}

/**
 * @brief Destroy the Path object
 */
Path::~Path() {}

/**
 * @brief Gets the list of states along a motion-profiled path in chronological order.
 *
 * @return a list of MotionProfilePoints in chronological order as a std::vector<MotionProfilePoint>
 */
std::vector<MotionProfilePoint> Path::getProfilePoints() {
    return this->profilePoints;
}

/**
 * @brief Determines if this Path is equal to the given Path
 *
 * @param other The Path to compare to
 */
bool Path::operator==(const Path &other) const {
    if (this->profilePoints.size() != other.profilePoints.size()) {
        return false;
    }
    for (int i = 0; i < this->profilePoints.size(); i++) {
        if (this->profilePoints[i] != other.profilePoints[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Determines if this Path is not equal to the given Path
 *
 * @param other The Path to compare to
 */
bool Path::operator!=(const Path &other) const { return !(*this == other); }

} // namespace src::Motion