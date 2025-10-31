#ifndef MATERIAL
#define MATERIAL

#include "Color.hpp" // Você já tem esta classe

class Material {
public:
  Color Ka; // Coeficiente Ambiente
  Color Kd; // Coeficiente Difuso
  Color Ks; // Coeficiente Especular

  float shininess; // Expoente especular (o "foco" do brilho)

  // Construtor padrão
  Material();

  Material(Color ka, Color kd, Color ks, float shiny);
};

#endif // !MATERIAL
