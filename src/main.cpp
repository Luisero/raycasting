#include "../include/Point.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Vector.hpp"
#include <array>
#include <fstream>
#include <iostream>

const float wJanela = 2.f, hJanela = 1.5f;
const int nCol = 2.f * 400, nLin = 1.5 * 400;
float Dx = wJanela / nCol;
float Dy = hJanela / nLin;
float dJanela = 10;

Point posicaoObservador(0, 0, 0, 1);

void convertDisplayParaJanela(int display_x, int display_y, float &ndc_x,
                              float &ndc_y) {
  // Adiciona 0.5 para obter o centro do pixel
  ndc_x = -wJanela / 2.0f + Dx / 2.0f + display_x * Dx;
  ndc_y = hJanela / 2.0f - Dy / 2.0f - display_y * Dy;
}

int main() {
  std::cout << "Hey!\n";
  std::ofstream image("image.ppm");

  float rEsfera = .2f;
  std::array<Object *, 1> spheres = {
      new Sphere(Point(0.f, 0.f, -dJanela + rEsfera, 1.f), rEsfera)};

  if (image.is_open()) {
    image << "P3\n";
    image << nCol << " " << nLin << "\n";
    image << 255 << "\n";

    for (int l = 0; l < nLin; l++) {
      for (int c = 0; c < nCol; c++) {
        float x, y;

        convertDisplayParaJanela(c, l, x, y);
        Vector4 d(x - posicaoObservador.x, y - posicaoObservador.y, -dJanela,
                  0);

        Ray ray(d, posicaoObservador);
        float discriminant = spheres[0]->intersect(ray);
      }
    }
  }
  return 0;
}
