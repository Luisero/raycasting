#include "../include/Cylinder.hpp"
#include "../include/Plane.hpp"    // Precisamos de Planos para as tampas
#include "../include/Object.hpp"  // Para getIntersectedObject
#include <cmath>
#include <algorithm> // Para std::min/std::max

Cylinder::Cylinder(Point base, Vector4 axis_dir, float r, float h, Material mat) {
  this->base_point = base;
  this->axis = axis_dir.normalized(); // Garante que o eixo está normalizado
  this->radius = r;
  this->height = h;
  this->material = mat;
  // 'center' da classe base pode ser a base
  this->center = base; 
}


float Cylinder::intersect(Ray ray) {
  Vector4 D = ray.dir;
  Vector4 V = this->axis;
  Vector4 oc = ray.origin - this->base_point;

  // --- 1. Interseção com o "Tubo" (Cilindro Infinito) ---
  // Usamos a fórmula quadrática baseada em produtos vetoriais
  // a*t^2 + b*t + c = 0

  Vector4 D_x_V = D.cross(V);
  Vector4 oc_x_V = oc.cross(V);

  float a = D_x_V.dot(D_x_V);
  float b = 2.0f * D_x_V.dot(oc_x_V);
  float c = oc_x_V.dot(oc_x_V) - (this->radius * this->radius);

  float discriminant = b * b - 4 * a * c;

  float closest_t = -1.0f;

  if (discriminant >= 0) { // O raio atinge o cilindro infinito
    float t0 = (-b - std::sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + std::sqrt(discriminant)) / (2.0f * a);

    // Verifica a altura da primeira interseção (t0)
    if (t0 > 0.001f) {
      Point P0 = ray.origin + (D * t0);
      float h0 = (P0 - this->base_point).dot(V);
      if (h0 >= 0 && h0 <= this->height) {
        closest_t = t0;
      }
    }

    // Verifica a altura da segunda interseção (t1)
    if (t1 > 0.001f) {
      Point P1 = ray.origin + (D * t1);
      float h1 = (P1 - this->base_point).dot(V);
      if (h1 >= 0 && h1 <= this->height) {
        if (closest_t == -1.0f || t1 < closest_t) {
          closest_t = t1;
        }
      }
    }
  }

  // --- 2. Interseção com as "Tampas" (Planos) ---

  // Tampa da Base
  Plane bottom_cap(this->base_point, this->axis * -1.0f, this->material);
  float t_cap_bottom = bottom_cap.intersect(ray);

  if (t_cap_bottom > 0.001f) {
    Point P_bottom = ray.origin + (D * t_cap_bottom);
    // Verifica se o ponto está dentro do raio da tampa
    if ((P_bottom - this->base_point).lenght() <= this->radius) {
      if (closest_t == -1.0f || t_cap_bottom < closest_t) {
        closest_t = t_cap_bottom;
      }
    }
  }

  // Tampa do Topo
  Point top_point = this->base_point + (V * this->height);
  Plane top_cap(top_point, this->axis, this->material);
  float t_cap_top = top_cap.intersect(ray);

  if (t_cap_top > 0.001f) {
    Point P_top = ray.origin + (D * t_cap_top);
    // Verifica se o ponto está dentro do raio da tampa
    if ((P_top - top_point).lenght() <= this->radius) {
      if (closest_t == -1.0f || t_cap_top < closest_t) {
        closest_t = t_cap_top;
      }
    }
  }

  return closest_t;
}


Vector4 Cylinder::getNormal(Point collide) {
  float epsilon = 0.001f;

  // Projeta o ponto de colisão no eixo do cilindro
  float height_on_axis = (collide - this->base_point).dot(this->axis);

  // --- Checa se atingiu as tampas ---
  
  // Tampa da Base
  if (std::abs(height_on_axis) < epsilon) {
    return this->axis * -1.0f; // Normal aponta "para baixo"
  }

  // Tampa do Topo
  if (std::abs(height_on_axis - this->height) < epsilon) {
    return this->axis; // Normal aponta "para cima"
  }

  // --- Atingiu a lateral (o "tubo") ---
  // A normal é o vetor do eixo para o ponto de colisão
  
  // 1. Encontra o ponto no eixo mais próximo do ponto de colisão
  Point point_on_axis = this->base_point + (this->axis * height_on_axis);
  
  // 2. O vetor normal é a diferença
  Vector4 normal = collide - point_on_axis;
  
  return normal.normalized();
}


// --- 3. Implementação da Função de Sombreamento ---
// (Este código é IDÊNTICO ao de Sphere.cpp e Plane.cpp)
Color Cylinder::shade(
    const Ray& viewingRay,
    const Point& P,
    const Point& lightPosition,
    const Color& lightIntensity,
    const Color& ambientLightIntensity,
    const Point& observerPosition,
    const std::vector<std::unique_ptr<Object>>& allObjects)
{
  Material mat = this->material;
  Vector4 N = this->getNormal(P);
  N.normalize(); // N é uma cópia, pode modificar

  Color ambientColor = mat.Ka * ambientLightIntensity;

  Vector4 lightVector = (lightPosition - P);
  float distanceToLight = lightVector.lenght();
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