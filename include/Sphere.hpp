#ifndef SPHERE
#define SPHERE

#include "./Object.hpp"

class Sphere : public Object {
public:
  float radius;
  Sphere();
  Sphere(Point center, float radius);

  virtual float intersect(Ray ray) override;
};

#endif // !SPHERE
