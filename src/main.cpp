#include "../include/Point.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Vector.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <memory>

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

  std::ofstream image("image.ppm");

  float rEsfera = .2f;
  Point defaultCenter(0.f, 0.f, -dJanela + rEsfera, 1.f);
  std::array<std::unique_ptr<Object>, 10> spheres = {
      std::make_unique<Sphere>(defaultCenter, rEsfera)};

  for (int i = 1; i < 10; i++) {
    spheres[i] = std::make_unique<Sphere>(
        defaultCenter + Vector4(-.4f / i, .4f, -0.5f, 0.f), rEsfera / i);
  }

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

        // É uma boa prática normalizar a direção

        Ray ray(d, posicaoObservador);

        // --- LÓGICA DE INTERSEÇÃO CORRIGIDA ---

        // 1. Inicie a distância mais próxima como "sem colisão" (-1.0f).
        float t_mais_proximo = -1.0f;

        // 2. Faça o loop por todos os objetos no array
        for (const auto &object : spheres) {

          // Pule se o slot do array estiver vazio (embora você tenha preenchido
          // todos)
          if (object == nullptr) {
            continue;
          }

          // 3. Chame intersect E armazene o resultado
          float t_atual = object->intersect(ray);

          // 4. Verifique se esta colisão é válida E é a mais próxima até agora
          if (t_atual > 0.001f) { // (t > 0.001f para evitar auto-colisão)

            if (t_mais_proximo == -1.0f || t_atual < t_mais_proximo) {
              // Se esta é a primeira colisão que encontramos (t_mais_proximo ==
              // -1) OU se a colisão atual (t_atual) é mais próxima que a
              // anterior, atualize t_mais_proximo.
              t_mais_proximo = t_atual;
            }
          }
        }

        // 5. Verifique o resultado final (t_mais_proximo)
        if (t_mais_proximo == -1.0f) {
          image << "10 50 200 "; // Cor de fundo (sem colisão)
        } else {
          // (Opcional, mas recomendado: calcule o ponto de colisão correto)
          // Point collidePoint = ray.origin + (ray.dir * t_mais_proximo);

          image << "200 100 50 "; // Cor do objeto (colisão!)
        }
      }
      image << "\n";
    }
    image.close();
  }
  return 0;
}
