#ifndef MIRROR_SPHERE_HPP // É bom usar um guarda de inclusão diferente
#define MIRROR_SPHERE_HPP

#include "Sphere.hpp"   // Inclui a classe base da qual estamos herdando
#include "Material.hpp" // Para o construtor
#include "Point.hpp"    // Para o construtor

/**
 * @brief Define uma Esfera-Espelho.
 * Ela herda de Sphere, mas é um TIPO distinto,
 * permitindo que o dynamic_cast funcione.
 */
class MirrorSphere : public Sphere {
public:
  MirrorSphere(Point center, float radius, Material mat);

  // --- Adiciona a declaração de override ---
  virtual Color shade(
      const Ray& viewingRay,
      const Point& intersectionPoint,
      const Point& lightPosition,
      const Color& lightIntensity,
      const Color& ambientLightIntensity,
      const Point& observerPosition,
      const std::vector<std::unique_ptr<Object>>& allObjects
  ) override;
};

#endif // !MIRROR_SPHERE_HPP