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
Color Sphere::shade(
    const Ray& viewingRay,
    const Point& P, // Ponto de interseção
    const Point& lightPosition,
    const Color& lightIntensity,
    const Color& ambientLightIntensity,
    const Point& observerPosition,
    const std::vector<std::unique_ptr<Object>>& allObjects)
{
  // Esta é a lógica de "Phong Shading" que estava em main.cpp
  Material mat = this->material;
  Vector4 N = this->getNormal(P);
  N.normalize(); // Modifica N, o que é OK.

  Color ambientColor = mat.Ka * ambientLightIntensity;

  Vector4 lightVector = (lightPosition - P);
  float distanceToLight = lightVector.lenght();
  // Use a nova função 'normalized()' que é const-safe
  Vector4 lightDirection = lightVector.normalized();

  // --- Shadow Check ---
  Point shadowRayOrigin = P + (N * 0.001f);
  Ray shadowRay(lightDirection, shadowRayOrigin);

  float shadow_t;
  Object *obstructingObject =
      getIntersectedObject(shadowRay, allObjects, shadow_t);

  bool inShadow = (obstructingObject != nullptr) &&
                  (shadow_t < distanceToLight);

  Color diffuseColor(0, 0, 0);
  Color specularColor(0, 0, 0);

  if (!inShadow) {
    Vector4 V = (observerPosition - P).normalized();
    Vector4 R =
        (N * (2.0f * N.dot(lightDirection))) - lightDirection;

    float diffuseFactor = std::max(0.0f, N.dot(lightDirection));
    if (diffuseFactor > 0) {
      diffuseColor = (mat.Kd * lightIntensity) * diffuseFactor;
    }

    float specularFactor = std::pow(
        std::max(0.0f, V.dot(R.normalized())), mat.shininess);
    if (specularFactor > 0) {
      specularColor = (mat.Ks * lightIntensity) * specularFactor;
    }
  }

  Color finalColor = ambientColor + diffuseColor + specularColor;
  return finalColor;
}