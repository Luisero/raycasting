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

  // --- Adiciona a declaração de override ---
  virtual Color shade(
      const Ray& viewingRay,
      const Point& intersectionPoint,
      const Point& lightPosition,
      const Color& lightIntensity,
      const Color& ambientLightIntensity,
      const Point& observerPosition,
      const std::vector<std::unique_ptr<Object>>& allObjects
  ) override;
};
#endif