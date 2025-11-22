#ifndef TRIANGLE
#define TRIANGLE

#include "Matrix4.hpp" // <-- Inclua a Matriz
#include "Object.hpp"
#include "Point.hpp"
#include "Vector.hpp"

class Triangle : public Object {
public:
  Point v0, v1, v2;
  Vector4 normal;

  Triangle(Point a, Point b, Point c, Material mat);

  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;
  virtual Color
  shade(const Ray &viewingRay, const Point &intersectionPoint,
        const Point &lightPosition, const Color &lightIntensity,
        const Color &ambientLightIntensity, const Point &observerPosition,
        const std::vector<std::unique_ptr<Object>> &allObjects) override;

  // --- NOVO MÉTODO ---
  // Aplica uma matriz de transformação aos vértices do triângulo
  void applyTransform(const Matrix4 &m);
};

#endif
