#include "path.hpp"

/**
 * @brief Construct a new Path object that represents a list of states along a motion-profiled path 
 * in chronological order.
 */
Path::Path() {
    this->profilePoints = std::list<MotionProfilePoint>();
}

/**
 * @brief Destroy the Path object
 */
Path::~Path(){}

/**
 * @brief Gets the list of states along a motion-profiled path in chronological order.
 * 
 * @return a list of MotionProfilePoints in chronological order as a std::list<MotionProfilePoint> 
 */
std::list<MotionProfilePoint> Path::getProfilePoints() {
    return this->profilePoints;
}