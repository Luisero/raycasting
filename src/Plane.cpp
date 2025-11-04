#include "../include/Plane.hpp"
#include <cmath> // Para std::abs

// Construtor padrão: um "chão" no nível Y=0
Plane::Plane() : Object() {
  this->point = Point(0, 0, 0, 1);      // Um ponto no plano (origem)
  this->normal = Vector4(0, 1, 0, 0); // Apontando para cima (eixo Y)
  this->material = Material();        // Material padrão
  this->center = this->point;         // A classe base tem 'center', vamos usá-lo
}

// Construtor principal
Plane::Plane(Point p, Vector4 n, Material mat) {
  this->point = p;
  this->normal = n.normalize(); // Garante que a normal está normalizada
  this->material = mat;
  this->center = p;
}

/**
 * Retorna a normal do plano.
 * Para um plano, a normal é a mesma em todos os pontos.
 */
Vector4 Plane::getNormal(Point collide) {
  // O parâmetro 'collide' é ignorado, pois a normal é constante.
  return this->normal;
}

/**
 * Calcula a interseção do raio com o plano.
 * Fórmula: t = ((P0 - Origem_Raio) . N) / (Direcao_Raio . N)
 */
float Plane::intersect(Ray ray) {
  
  // Denominador: (Direcao_Raio . N)
  // [baseado em uploaded:raycasting/include/Vector.hpp]
  float denominator = ray.dir.dot(this->normal); 

  // Se o denominador for quase zero, o raio é paralelo ao plano.
  // Usamos 1e-6 (ou 0.000001) como um "epsilon" para evitar divisão
  // por zero ou problemas de ponto flutuante.
  if (std::abs(denominator) > 1e-6f) {
    
    // Numerador: (P0 - Origem_Raio) . N
    // (P0 é this->point)
    Vector4 p0_o = this->point - ray.origin;
    float t = p0_o.dot(this->normal) / denominator;

    // Se t > 0.001f, a interseção está na *frente* do raio
    // (Usamos 0.001f para evitar auto-interseção)
    if (t > 0.001f) {
      return t;
    }
  }

  // Nenhuma interseção (ou é paralelo ou está atrás do raio)
  return -1.0f;
}

Color Plane::shade(
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
  N.normalize(); // N é uma cópia, pode modificar

  Color ambientColor = mat.Ka * ambientLightIntensity;

  Vector4 lightVector = (lightPosition - P);
  float distanceToLight = lightVector.lenght();
  Vector4 lightDirection = lightVector.normalized(); // Usa a versão const-safe

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
    Vector4 V = (observerPosition - P).normalized(); // Usa const-safe
    Vector4 R =
        (N * (2.0f * N.dot(lightDirection))) - lightDirection;

    float diffuseFactor = std::max(0.0f, N.dot(lightDirection));
    if (diffuseFactor > 0) {
      diffuseColor = (mat.Kd * lightIntensity) * diffuseFactor;
    }

    float specularFactor = std::pow(
        std::max(0.0f, V.dot(R.normalized())), mat.shininess); // Usa const-safe
    if (specularFactor > 0) {
      specularColor = (mat.Ks * lightIntensity) * specularFactor;
    }
  }

  // Cor final é ambiente + (difusa + especular se não estiver na sombra)
  Color finalColor = ambientColor + diffuseColor + specularColor;
  return finalColor;
}