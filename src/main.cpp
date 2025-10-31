#include "../include/Color.hpp"
#include "../include/Point.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Vector.hpp"
#include "../include/Plane.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <memory>

const float wJanela = 2.f, hJanela = 1.5f;
const int nCol = 2.f * 1920, nLin = 1.5 * 1920;
float Dx = wJanela / nCol;
float Dy = hJanela / nLin;
float dJanela = 10;

Point posicaoObservador(0, 0, 0, 1);

Point luz_pos(2.f, 2.0f, -dJanela/1.5, 1.0f);
Color luz_intensidade(255, 255, 255);       // Luz branca
Color luz_ambiente_intensidade(25, 25, 25); // Luz ambiente fraca

void convertDisplayParaJanela(int display_x, int display_y, float &ndc_x,
                              float &ndc_y) {
  // Adiciona 0.5 para obter o centro do pixel
  ndc_x = -wJanela / 2.0f + Dx / 2.0f + display_x * Dx;
  ndc_y = hJanela / 2.0f - Dy / 2.0f - display_y * Dy;
}

int main() {

  std::ofstream image("image.ppm");
  Material mat_laranja(Color(10, 5, 2),      // Ka (ambiente escuro)
                       Color(200, 100, 50),  // Kd (cor principal)
                       Color(255, 255, 255), // Ks (brilho branco)
                       128.0f);
  Material mat_chao(Color(0, 30, 00),     // Ka (ambiente)
                       Color(35, 166, 12), // Kd (difuso cinza)
                       Color(255, 255, 255),    // Ks (pouco brilho)
                       255.f);               // shininess
              
  float rEsfera = .2f;
  Point defaultCenter(0.f, 0.f, -dJanela + rEsfera, 1.f);
  std::array<std::unique_ptr<Object>, 10> objects = {
      std::make_unique<Sphere>(defaultCenter, rEsfera, mat_laranja)};

  for (int i = 1; i < 10; i++) {
   Material mat(
      Color(10,10,10),
      Color(i*10, i*20,50),
      Color(255,255,255),
      5.f
   );
    objects[i] = std::make_unique<Sphere>(
        defaultCenter + Vector4(i/5 , .4f, -i, 0.f), rEsfera/3,
        mat);
  }
  Point ponto_do_chao(0.f, -rEsfera * 2.f, 0.f, 1.f); // Ponto no chão
  Vector4 normal_do_chao(0.f, 1.f, 0.f, 0.f);      // Apontando p/ cima
  objects[9] = std::make_unique<Plane>(ponto_do_chao, normal_do_chao, mat_chao);

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
        Object *obj_mais_proximo = nullptr; // <-- PRECISAMOS GUARDAR O OBJETO
        // 2. Faça o loop por todos os objetos no array
        for (const auto &object : objects) {

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
              obj_mais_proximo =
                  object.get(); // <-- Salva o ponteiro p/ o objeto
            }
          }
        }

        // 5. Verifique o resultado final (t_mais_proximo)
        if (t_mais_proximo == -1.0f) {
          image << "10 50 200 "; // Cor de fundo (sem colisão)
        } else {
          // (Opcional, mas recomendado: calcule o ponto de colisão correto)
          // Point collidePoint = ray.origin + (ray.dir * t_mais_proximo);
          Point p = ray.origin + (ray.dir * t_mais_proximo);
          Vector4 N = obj_mais_proximo->getNormal(p);
          N.normalize();
          Material mat = obj_mais_proximo->material;

          Color cor_ambient = mat.Ka * luz_ambiente_intensidade;

          Vector4 L = (luz_pos - p);
          L.normalize();
          Vector4 V = (posicaoObservador - p);
          V.normalize();
          Vector4 R = (N * (2.0f * N.dot(L))) - L;

          float fator_difuso = std::max(0.0f, N.dot(L));
          Color cor_difusa(0, 0, 0);
          if (fator_difuso > 0) {
            cor_difusa = (mat.Kd * luz_intensidade) * fator_difuso;
          }

          float fator_especular =
              std::pow(std::max(0.0f, V.dot(R)), mat.shininess);
          Color cor_especular(0, 0, 0);
          if (fator_especular > 0) {
            cor_especular = (mat.Ks * luz_intensidade) * fator_especular;
          }
          Color cor_final = cor_ambient + cor_difusa + cor_especular;
          cor_final.clamp(); // Garante que não passa de 255

          image << cor_final.r << " " << cor_final.g << " " << cor_final.b
                << " ";
        }
      }
      image << "\n";
    }
    image.close();
  }
  return 0;
}
