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
  // Construtor que simplesmente repassa os dados para a classe base Sphere
  MirrorSphere(Point center, float radius, Material mat);

  // Não precisamos de mais nada, apenas o fato de ser um 
  // tipo diferente já é o suficiente para o dynamic_cast.
};

#endif // !MIRROR_SPHERE_HPP