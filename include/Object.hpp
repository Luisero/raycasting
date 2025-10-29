#ifndef OBJECT
#define OBJECT
#include "./Point.hpp"
#include "./Ray.hpp"
#include "Vector.hpp"
class Object {
public:
  Point center;
  Object();
  virtual ~Object();
  virtual float intersect(Ray ray) = 0;
  virtual Vector4 getNormal(Point collide);
};
#endif
