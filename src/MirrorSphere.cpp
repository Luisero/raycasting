#include "../include/MirrorSphere.hpp"
#include "../include/Object.hpp" // <-- INCLUA PARA getIntersectedObject
#include <cmath> // <-- Certifique-se de que cmath está incluído (para std::max, etc.)

MirrorSphere::MirrorSphere(Point center, float radius, Material mat)
    : Sphere(center, radius, mat) {}

// --- IMPLEMENTAÇÃO DO SOMBREAMENTO DE REFLEXÃO ---
Color MirrorSphere::shade(
    const Ray& viewingRay,
    const Point& P, // Ponto de interseção no espelho
    const Point& lightPosition,
    const Color& lightIntensity,
    const Color& ambientLightIntensity,
    const Point& observerPosition,
    const std::vector<std::unique_ptr<Object>>& allObjects)
{
    // 1. Calcule a direção do raio refletido
    Vector4 N = this->getNormal(P);
    N.normalize();
    Vector4 D = viewingRay.dir.normalized();

    float d_dot_n = D.dot(N);
    Vector4 reflectedDirection = D - (N * (2.0f * d_dot_n));
    reflectedDirection.normalize(); 

    // 2. Crie o raio refletido
    Point reflectedOrigin = P + (N * 0.001f);
    Ray reflected_ray(reflectedDirection, reflectedOrigin);

    // 3. Trace o raio refletido
    float reflected_t;
    Object *reflectedObject =
        getIntersectedObject(reflected_ray, allObjects, reflected_t);
    
    if (reflectedObject == nullptr) {
      return Color(10, 50, 200); // Raio refletido atingiu o fundo
    } else {
      // --- MUDANÇA PRINCIPAL AQUI ---
      
      // 4. Calcule o novo ponto de interseção (onde o raio refletido atingiu o objeto)
      Point reflectedIntersectionPoint = reflected_ray.origin + (reflected_ray.dir * reflected_t);

      // 5. Chame a função 'shade' DO OUTRO OBJETO
      // Para este novo cálculo:
      // - O "raio" é o raio refletido.
      // - O "observador" é o ponto P no espelho.
      return reflectedObject->shade(
          reflected_ray,              // O raio que o atingiu
          reflectedIntersectionPoint,   // O novo ponto de interseção
          lightPosition,                // A luz da cena (pass-through)
          lightIntensity,               // A intensidade da luz (pass-through)
          ambientLightIntensity,        // A luz ambiente (pass-through)
          P,                            // O "observador" é o ponto no espelho
          allObjects                    // A lista de todos os objetos (pass-through)
      );
    }
}