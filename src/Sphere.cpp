#include "../include/Sphere.hpp"
#include "../include/Material.hpp"
#include "../include/Vector.hpp"
#include <cmath>
Sphere::Sphere() : Object() {
  this->radius = 1.0;
  this->material = Material();
}

Sphere::Sphere(Point center, float radius, Material material) {
  this->center = center;
  this->radius = radius;
  this->material = material;
}

float Sphere::intersect(Ray ray) {

  Vector4 oc = ray.origin - this->center;

  float a = ray.dir.dot(ray.dir);
  float b = 2.0 * oc.dot(ray.dir);
  float c = oc.dot(oc) - this->radius * this->radius;

  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0; // Sem interseção
  } else {
    // Retorna a interseção mais próxima (menor t positivo)
    float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
    if (t1 > 0.001) { // 0.001 para evitar auto-interseção
      return t1;
    }
    float t2 = (-b + sqrt(discriminant)) / (2.0 * a);
    if (t2 > 0.001) {
      return t2;
    }
    return -1.0; // Ambas interseções estão atrás do raio
  }
}
Vector4 Sphere::getNormal(Point collide) {
  Vector4 normal = collide - this->center;
  normal.normalize();
  return normal;
}
