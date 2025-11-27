#ifndef MESH_HPP
#define MESH_HPP

#include "Matrix4.hpp"
#include "Object.hpp" // Herda de Object
#include "Point.hpp"
#include "Triangle.hpp" // Guarda Triangles
#include <memory>
#include <string>
#include <vector>

class Mesh : public Object {
public:
  std::vector<std::unique_ptr<Triangle>> triangles;
  std::vector<Point> vertices;

  Point minBound;
  Point maxBound;

  Mesh(); // Construtor

  // --- Métodos Obrigatórios de Object ---
  virtual float intersect(Ray ray) override;
  virtual Vector4
  getNormal(Point collide) override; // (Não usado diretamente, mas obrigatório)
  virtual Color
  shade(const Ray &viewingRay, const Point &intersectionPoint,
        const Point &lightPosition, const Color &lightIntensity,
        const Color &ambientLightIntensity, const Point &observerPosition,
        const std::vector<std::unique_ptr<Object>> &allObjects) override;

  // --- Métodos da Malha ---
  bool loadOBJ(const std::string &filename, Material mat);
  void applyTransform(const Matrix4 &transform);
  Point getCentroid() const;

private:
  // Métodos internos para gerenciar a caixa
  void calculateBounds();
  bool intersectAABB(Ray ray, float &tMinOut, float &tMaxOut);
};

#endif
