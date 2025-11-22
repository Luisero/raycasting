#ifndef CYLINDER
#define CYLINDER

#include "./Object.hpp"
#include "Vector.hpp"
#include "Point.hpp"
#include "Material.hpp"
#include <vector>
#include <memory>

class Cylinder : public Object {
public:
  float radius;
  float height;
  Point base_point; // O centro da base do cilindro
  Vector4 axis;     // O vetor de direção (eixo) do cilindro

  Cylinder(Point base, Vector4 axis_dir, float r, float h, Material mat);

  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;

  // A função 'shade' é idêntica à da Esfera/Plano
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

#endif // !CYLINDER