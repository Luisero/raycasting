#ifndef PLANE
#define PLANE

#include "./Object.hpp"
#include "Vector.hpp"
#include "Point.hpp"
#include "Material.hpp"
#include <vector>   // <-- Inclua, se necessário
#include <memory>   // <-- Inclua, se necessário

class Plane : public Object {
public:
  Vector4 normal;
  Point point;

  Plane();
  Plane(Point p, Vector4 n, Material mat);

  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;

  // --- ADICIONE ESTA DECLARAÇÃO ---
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

#endif // !PLANE