#ifndef OBJECT
#define OBJECT
#include "./Material.hpp"
#include "./Point.hpp"
#include "./Ray.hpp"
#include "Vector.hpp"
class Object {
public:
  Point center;
  Material material;
  Object();
  virtual ~Object();
  virtual float intersect(Ray ray) = 0;
  virtual Vector4 getNormal(Point collide);
};
#endif
