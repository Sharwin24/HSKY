#include "motionprofilepoint.hpp"
#include <list>

class Path {
    public:
        Path();
        ~Path();
        std::list<MotionProfilePoint> getProfilePoints();

    private: 
        std::list<MotionProfilePoint> profilePoints;
};