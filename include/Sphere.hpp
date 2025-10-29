#ifndef SPHERE
#define SPHERE

#include "./Object.hpp"
#include "Vector.hpp"

class Sphere : public Object {
public:
  float radius;
  Sphere();
  Sphere(Point center, float radius);

  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;
};

#endif // !SPHERE
