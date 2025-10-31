#ifndef PLANE
#define PLANE

#include "./Object.hpp"
#include "Vector.hpp"
#include "Point.hpp"
#include "Material.hpp"

class Plane : public Object {
public:
  Vector4 normal; // A normal da direção do plano
  Point point;    // Um ponto qualquer que pertence ao plano

  Plane();
  Plane(Point p, Vector4 n, Material mat);

  // Sobrescreve as funções virtuais da classe Object
  virtual float intersect(Ray ray) override;
  virtual Vector4 getNormal(Point collide) override;
};

#endif // !PLANE