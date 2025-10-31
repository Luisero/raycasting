#include "../include/Material.hpp"

// Um construtor padrão razoável (ex: gesso branco fosco)
Material::Material() {
  // Use os construtores da sua classe Color
  this->Ka = Color(5, 5, 5);       // Ambiente muito escuro
  this->Kd = Color(200, 200, 200); // Difuso quase branco
  this->Ks = Color(0, 0, 0);       // Sem brilho especular
  this->shininess = 1.0f;          // Brilho muito espalhado (quase inútil)
}

Material::Material(Color ka, Color kd, Color ks, float shiny) {
  this->Ka = ka;
  this->Kd = kd;
  this->Ks = ks;
  this->shininess = shiny;
}
