#pragma once
#include "robotpose.hpp"

namespace src::Motion {

class Vector {
  public:
    Vector(float x1, float y1, float x2, float y2)
        : x1(x1),
          y1(y1),
          x2(x2),
          y2(y2){};

    Vector(RobotPose start, RobotPose end)
        : x1(start.getXPosition()),
          y1(start.getYPosition()),
          x2(end.getXPosition()),
          y2(end.getYPosition()){};
    ~Vector();

    // Getters
    float getX1() const { return this->x1; };
    float getY1() const { return this->y1; };
    float getX2() const { return this->x2; };
    float getY2() const { return this->y2; };

    // Operators
    bool operator==(const Vector &other) const;
    bool operator!=(const Vector &other) const;
    Vector operator+(const Vector &other) const;
    Vector operator-(const Vector &other) const;
    Vector operator*(const float &other) const;
    Vector operator/(const float &other) const;

    float getMagnitude();
    float getAngle();
    Vector normalize();

  private:
    float x1;
    float y1;
    float x2;
    float y2;
};

} // namespace src::Motion