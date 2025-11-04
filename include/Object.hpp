#ifndef OBJECT
#define OBJECT
#include "./Material.hpp"
#include "./Point.hpp"
#include "./Ray.hpp"
#include "./Vector.hpp"
#include "./Color.hpp" // <-- INCLUA ISSO
#include <vector>     // <-- INCLUA ISSO
#include <memory>     // <-- INCLUA ISSO

class Object {
public:
  Point center;
  Material material;
  Object();
  virtual ~Object();
  virtual float intersect(Ray ray) = 0;
  virtual Vector4 getNormal(Point collide);

  // --- NOVA FUNÇÃO DE SOMBREAMENTO VIRTUAL ---
  // Define a "interface" de sombreamento. Cada filho DEVE implementar isso.
  virtual Color shade(
      const Ray& viewingRay,
      const Point& intersectionPoint,
      const Point& lightPosition,
      const Color& lightIntensity,
      const Color& ambientLightIntensity,
      const Point& observerPosition,
      const std::vector<std::unique_ptr<Object>>& allObjects
  ) = 0; // = 0 torna "pura virtual"
};

// --- DECLARAÇÃO DA FUNÇÃO GLOBAL ---
// Declara a função para que outros arquivos .cpp possam usá-la
Object *
getIntersectedObject(Ray ray,
                     const std::vector<std::unique_ptr<Object>> &objects,
                     float &closest_t);
#endif