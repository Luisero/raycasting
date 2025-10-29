#ifndef RAY
#define RAY
#include "./Point.hpp"
#include "./Vector.hpp"
class Ray {
public:
  Vector4 dir;
  Point origin;
  Ray(Vector4 dir, Point origin) {
    this->dir = dir;
    this->origin = origin;
  };
};
#endif // !DEBUG
