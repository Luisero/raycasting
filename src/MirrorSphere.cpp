#include "../include/MirrorSphere.hpp"

/**
 * @brief Construtor da MirrorSphere.
 * Ele apenas chama o construtor da classe base Sphere.
 */
MirrorSphere::MirrorSphere(Point center, float radius, Material mat)
    : Sphere(center, radius, mat) // Sintaxe de chamada do construtor base
{
  // O construtor da classe Sphere já faz todo o trabalho.
  // O corpo pode ficar vazio.
}