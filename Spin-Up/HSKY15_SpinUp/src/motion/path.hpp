#include "motionprofilepoint.hpp"
#include <list>

/**
 * @brief A class to represent a list of states along a motion-profiled path in chronological order.
 */
class Path {
    public:
        Path();
        ~Path();
        std::list<MotionProfilePoint> getProfilePoints();

    private: 
        std::list<MotionProfilePoint> profilePoints;
};