#ifndef SPHERE
#define SPHERE

#include "./Material.hpp"
#include "./Object.hpp"
#include "Vector.hpp"
class Sphere : public Object {
public:
  float radius;
  Sphere();
  Sphere(Point center, float radius, Material mat);

  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;
};

#endif // !SPHERE
