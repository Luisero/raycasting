#ifndef MESH_HPP
#define MESH_HPP

#include "Matrix4.hpp"
#include "Object.hpp"
#include "Point.hpp"
#include "Triangle.hpp"
#include <algorithm> // Para sort
#include <deque>
#include <memory>
#include <string>
#include <vector>

// Estrutura do Nó do BVH
struct BVHNode {
  Point minBox, maxBox;
  std::unique_ptr<BVHNode> left;
  std::unique_ptr<BVHNode> right;
  std::vector<Triangle *> leafTriangles; // Apenas folhas têm triângulos

  bool isLeaf() const { return !left && !right; }
};

class Mesh : public Object {
public:
  std::vector<std::unique_ptr<Triangle>> triangles;
  std::deque<Point> modelVertices;
  std::deque<Point> vertices;
  std::deque<Vector2> uvs;

  // Caixa delimitadora global da Mesh
  Point minBound;
  Point maxBound;

  // Raiz da Árvore BVH
  std::unique_ptr<BVHNode> bvhRoot;

  Mesh();

  // --- Métodos de Object ---
  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;
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
  void calculateBounds();

  // BVH Helpers
  void buildBVH();
  std::unique_ptr<BVHNode> recursiveBuild(std::vector<Triangle *> &tris);
  bool intersectBVH(BVHNode *node, const Ray &ray, float &closest_t,
                    Triangle *&hitTri);

  // Função estática para testar qualquer caixa (não só a da Mesh)
  static bool testAABB(const Ray &ray, const Point &minB, const Point &maxB);
};

#endif
