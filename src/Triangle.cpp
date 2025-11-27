#include "../include/Triangle.hpp"
#include "../include/Object.hpp" // Para getIntersectedObject
#include <algorithm>
#include <cmath>

Triangle::Triangle(Point a, Point b, Point c, Material mat) {
  this->v0 = a;
  this->v1 = b;
  this->v2 = c;
  this->material = mat;

  // --- Pré-calcula a Normal ---
  // Aresta 1: v1 - v0
  // Aresta 2: v2 - v0
  // Normal = Aresta1 X Aresta2 (Produto Vetorial)
  Vector4 edge1 = v1 - v0;
  Vector4 edge2 = v2 - v0;
  this->normal = edge1.cross(edge2).normalized();

  // Define um centro aproximado (baricentro) para a classe base, se necessário
  this->center = Point((v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3,
                       (v0.z + v1.z + v2.z) / 3, 1.0f);
}

Vector4 Triangle::getNormal(Point collide) {
  // Para um triângulo plano, a normal é constante em toda a face
  return this->normal;
}

// --- Algoritmo Möller–Trumbore ---
float Triangle::intersect(Ray ray) {
  if (ray.dir.dot(this->normal) >= 0)
    return -1;
  const float EPSILON = 0.0000001f;

  Vector4 edge1 = v1 - v0;
  Vector4 edge2 = v2 - v0;

  Vector4 h = ray.dir.cross(edge2);
  float a = edge1.dot(h);

  // Se 'a' for próximo de 0, o raio é paralelo ao triângulo
  if (a > -EPSILON && a < EPSILON) {
    return -1.0f;
  }

  float f = 1.0f / a;
  Vector4 s = ray.origin - v0;
  float u = f * s.dot(h);

  // Verifica se a interseção está fora do triângulo (coord. baricêntrica u)
  if (u < 0.0f || u > 1.0f) {
    return -1.0f;
  }

  Vector4 q = s.cross(edge1);
  float v = f * ray.dir.dot(q);

  // Verifica se a interseção está fora do triângulo (coord. baricêntrica v)
  if (v < 0.0f || u + v > 1.0f) {
    return -1.0f;
  }

  // Neste ponto, temos uma linha de interseção. Vamos descobrir o 't'.
  float t = f * edge2.dot(q);

  if (t > EPSILON) { // Interseção válida
    return t;
  } else { // Interseção atrás da origem do raio
    return -1.0f;
  }
}

// Copie a função shade do Plane.cpp ou Sphere.cpp, a lógica Phong é idêntica
Color Triangle::shade(const Ray &viewingRay, const Point &P,
                      const Point &lightPosition, const Color &lightIntensity,
                      const Color &ambientLightIntensity,
                      const Point &observerPosition,
                      const std::vector<std::unique_ptr<Object>> &allObjects) {
  // ... (Coloque aqui o mesmo código de shade do Plane/Sphere) ...
  // Lembre-se de usar this->getNormal(P)

  Material mat = this->material;
  Vector4 N = this->getNormal(P);
  N.normalize(); // N é uma cópia, pode modificar

  Color ambientColor = mat.Ka * ambientLightIntensity;

  Vector4 lightVector = (lightPosition - P);
  float distanceToLight = lightVector.lenght();
  Vector4 lightDirection = lightVector.normalized(); // Usa a versão const-safe

  // --- Shadow Check ---
  Point shadowRayOrigin = P + (N * 0.01f);
  Ray shadowRay(lightDirection, shadowRayOrigin);

  float shadow_t;
  Object *obstructingObject =
      getIntersectedObject(shadowRay, allObjects, shadow_t);

  bool inShadow =
      (obstructingObject != nullptr) && (shadow_t < distanceToLight);

  Color diffuseColor(0, 0, 0);
  Color specularColor(0, 0, 0);

  if (!inShadow) {
    Vector4 V = (observerPosition - P).normalized(); // Usa const-safe
    Vector4 R = (N * (2.0f * N.dot(lightDirection))) - lightDirection;

    float diffuseFactor = std::max(0.0f, N.dot(lightDirection));
    if (diffuseFactor > 0) {
      diffuseColor = (mat.Kd * lightIntensity) * diffuseFactor;
    }

    float specularFactor = std::pow(std::max(0.0f, V.dot(R.normalized())),
                                    mat.shininess); // Usa const-safe
    if (specularFactor > 0) {
      specularColor = (mat.Ks * lightIntensity) * specularFactor;
    }
  }

  // Cor final é ambiente + (difusa + especular se não estiver na sombra)
  Color finalColor = ambientColor + diffuseColor + specularColor;
  return finalColor;
}

void Triangle::applyTransform(const Matrix4 &m) {
  // 1. Transforma os 3 vértices
  // O operador * (Matriz * Vetor) já lida com Point corretamente se w=1
  Vector4 newV0 = m * Vector4(v0.x, v0.y, v0.z, 1.0f);
  Vector4 newV1 = m * Vector4(v1.x, v1.y, v1.z, 1.0f);
  Vector4 newV2 = m * Vector4(v2.x, v2.y, v2.z, 1.0f);

  v0 = Point(newV0.x, newV0.y, newV0.z, 1.0f);
  v1 = Point(newV1.x, newV1.y, newV1.z, 1.0f);
  v2 = Point(newV2.x, newV2.y, newV2.z, 1.0f);

  // 2. Recalcula a Normal (a rotação pode ter mudado a orientação)
  Vector4 edge1 = v1 - v0;
  Vector4 edge2 = v2 - v0;
  this->normal = edge1.cross(edge2).normalized();

  // 3. Recalcula o Centro (para animações genéricas que usem obj->center)
  this->center = Point((v0.x + v1.x + v2.x) / 3.0f, (v0.y + v1.y + v2.y) / 3.0f,
                       (v0.z + v1.z + v2.z) / 3.0f, 1.0f);
}
