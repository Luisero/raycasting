#ifndef COLOR
#define COLOR
#include <algorithm>

class Color {
public:
  int r, g, b;
  Color();
  Color(int r, int g, int b);
  Color operator+(Color c) {
    return Color(this->r + c.r, this->g + c.g, this->b + c.b);
  }
  Color operator*(Color c) {
    // Normalizamos para [0, 1] para multiplicar, depois voltamos para [0, 255]
    float r1 = this->r / 255.0f;
    float g1 = this->g / 255.0f;
    float b1 = this->b / 255.0f;

    float r2 = c.r / 255.0f;
    float g2 = c.g / 255.0f;
    float b2 = c.b / 255.0f;

    return Color((r1 * r2) * 255, (g1 * g2) * 255, (b1 * b2) * 255);
  }

  // Multiplicação por escalar (Cor * float)
  Color operator*(float f) {
    return Color(this->r * f, this->g * f, this->b * f);
  }

  // Garante que a cor final não passe de 255
  void clamp() {
    r = std::min(255, std::max(0, r));
    g = std::min(255, std::max(0, g));
    b = std::min(255, std::max(0, b));
  }
};
#endif // !COLOR
