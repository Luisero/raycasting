#include "../include/Object.hpp"

Object::Object() { this->center = Point(0.0f, 0.0f, 0.0f, 1.0f); }
Object::~Object() {}

Vector4 Object::getNormal(Point collide) { return Vector4(0.f, 0.f, 0.f, 0.f); }


// --- IMPLEMENTAÇÃO DA FUNÇÃO GLOBAL ---
// Movemos esta função de main.cpp para cá
Object *
getIntersectedObject(Ray ray,
                     const std::vector<std::unique_ptr<Object>> &objects,
                     float &closest_t) {
  closest_t = -1.0f;
  Object *closestObject = nullptr;

  for (const auto &object : objects) {
    if (object == nullptr) {
      continue;
    }
    float current_t = object->intersect(ray);

    if (current_t > 0.001f) // Epsilon to avoid self-intersection
    {
      if (closest_t == -1.0f || current_t < closest_t) {
        closest_t = current_t;
        closestObject = object.get();
      }
    }
  }
  return closestObject;
}