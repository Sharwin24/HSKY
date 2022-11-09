#include "vector.hpp"
#include "math.h"

namespace src::Motion {

float Vector::getMagnitude() {
    return sqrt(pow(x2 - x1, 2.0f) + pow(y2 - y1, 2.0f));
}

float Vector::getAngle() {
    // NOTE: Ensure this returns the same angle as RobotPose getTheta would
    return atan2(y2 - y1, x2 - x1);
}

Vector Vector::normalize() {
    float magnitude = this->getMagnitude();
    return Vector(x1 / magnitude, y1 / magnitude, x2 / magnitude, y2 / magnitude);
}

bool Vector::operator==(const Vector &other) const {
    return this->x1 == other.x1 && this->y1 == other.y1 && this->x2 == other.x2 && this->y2 == other.y2;
}

bool Vector::operator!=(const Vector &other) const { return !(*this == other); }

Vector Vector::operator+(const Vector &other) const {
    return Vector(this->x1 + other.x1, this->y1 + other.y1, this->x2 + other.x2, this->y2 + other.y2);
}

Vector Vector::operator-(const Vector &other) const {
    return Vector(this->x1 - other.x1, this->y1 - other.y1, this->x2 - other.y2, this->y2 - other.y2);
}

Vector Vector::operator*(const float &other) const {
    return Vector(this->x1 * other, this->y1 * other, this->x2 * other, this->y2 * other);
}

Vector Vector::operator/(const float &other) const {
    return Vector(this->x1 / other, this->y1 / other, this->x2 / other, this->y2 / other);
}

} // namespace src::Motion