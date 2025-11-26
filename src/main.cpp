#include "../include/Color.hpp"
#include "../include/Cylinder.hpp"
#include "../include/Matrix4.hpp"
#include "../include/Mesh.hpp"
#include "../include/MirrorSphere.hpp"
#include "../include/Object.hpp"
#include "../include/Plane.hpp"
#include "../include/Point.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Triangle.hpp"
#include "../include/Vector.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <omp.h> // Biblioteca OpenMP
#include <vector>

// --- CONSTANTES ---
const float windowWidth = 2.f, windowHeight = 1.5f;
// Aumentei um pouco a resolução baseada na sua lógica
const int numCols = windowWidth * 50;  // ~600 px
const int numRows = windowHeight * 50; // ~450 px
float Dx = windowWidth / numCols;
float Dy = windowHeight / numRows;
float viewplaneDistance = 10;
const int FRAMES_AMOUNT = 1; // 3 segundos de animação (30 * 3)

Point observerPosition(0, 0, 0, 1);

Point lightPosition(-1.f, 1.0f, -viewplaneDistance / 4, 1.0f);
Color lightIntensity(255, 255, 255);
Color ambientLightIntensity(80, 80, 80);

void convertDisplayToWindow(int display_x, int display_y, float &ndc_x,
                            float &ndc_y) {
  ndc_x = -windowWidth / 2.0f + Dx / 2.0f + display_x * Dx;
  ndc_y = windowHeight / 2.0f - Dy / 2.0f - display_y * Dy;
}

// A função getIntersectedObject está em Object.cpp

int main() {
  float time = 0;

  // --- MATERIAIS ---
  Material matOrange(Color(10, 5, 2), Color(200, 100, 50), Color(255, 255, 255),
                     128.0f);
  Material matPink(Color(10, 5, 2), Color(255, 141, 161), Color(255, 255, 255),
                   128.0f);
  Material matRed(Color(10, 5, 2), Color(255, 10, 20), Color(255, 255, 255),
                  128.0f);
  Material matFloor(Color(40, 90, 50), Color(35, 196, 12), Color(255, 255, 255),
                    255.f);
  Material matWall(Color(40, 40, 50), Color(35, 100, 196), Color(255, 255, 255),
                   99999.f);
  Material matMirror(Color(0, 0, 0), Color(0, 0, 0), Color(0, 0, 0), 128.f);

  Material matCube(Color(93, 99, 107),   // Ka (Ambiente: marrom escuro)
                   Color(185, 192, 201), // Kd (Difuso: SaddleBrown)
                   Color(255, 255, 255), // Ks (Branco)
                   64.0f                 // Shininess
  );

  float sphereRadius = .2f;
  Point defaultCenter(0.f, 0.f, -viewplaneDistance + sphereRadius, 1.f);

  std::vector<std::unique_ptr<Object>> objects;

  // --- CARREGAMENTO DA MALHA (Lógica AABB) ---
  // 1. Criamos o ponteiro inteligente
  auto meshPtr = std::make_unique<Mesh>();
  // 2. Guardamos uma referência crua (raw pointer) para poder animar depois
  Mesh *bunnyMesh = meshPtr.get();

  // 3. Carregamos o arquivo
  if (bunnyMesh->loadOBJ("ourladuguadalupe.obj", matCube)) {
    // Configuração inicial
    Matrix4 setupMatrix =
        Matrix4::translate(-125.5f, -101.5f, -viewplaneDistance * 3.5f);
    //        Matrix4::rotateX(-3.14f);
    bunnyMesh->applyTransform(setupMatrix);
    Point p = bunnyMesh->getCentroid();
    bunnyMesh->applyTransform(Matrix4::translate(-p.x, -p.y, -p.z));
    bunnyMesh->applyTransform(Matrix4::scale(.004f, .004f, .004f));
    bunnyMesh->applyTransform(Matrix4::translate(p.x, p.y, p.z));
    // 4. Movemos a posse da malha para a lista de objetos
    objects.push_back(std::move(meshPtr));
  } else {
    std::cerr << "Erro: bunny.obj nao encontrado!\n";
  }

  // --- OUTROS OBJETOS ---
  objects.push_back(std::make_unique<MirrorSphere>(
      Point(-0.5f, 0.f, -viewplaneDistance, 1), sphereRadius, matRed));

  Point floorPoint(0.f, -sphereRadius * 2.f, 0.f, 1.f);
  Vector4 floorNormal(0.f, 1.f, 0.f, 0.f);
  objects.push_back(std::make_unique<Plane>(floorPoint, floorNormal, matFloor));

  Point wallPoint(0.f, 0.f, -viewplaneDistance * 2, 1.f);
  Vector4 wallNormal(0, 0, 1, 0);
  //  objects.push_back(std::make_unique<Plane>(wallPoint, wallNormal,
  //  matWall));

  // --- LOOP PRINCIPAL ---
  for (int i = 0; i < FRAMES_AMOUNT; i++) {
    std::cout << "Rendering frame " << i << " / " << FRAMES_AMOUNT << "...\n";
    time += 0.1;

    std::string frametitle = "image";
    frametitle.append(std::to_string(i));
    frametitle.append(".ppm");
    std::ofstream image(frametitle);

    if (image.is_open()) {
      image << "P3\n" << numCols << " " << numRows << "\n" << 255 << "\n";

      // --- 1. ANIMAÇÃO ---

      // Objetos simples
      for (const auto &obj : objects) {
        if (!obj)
          continue;
        if (MirrorSphere *mirrorSphere =
                dynamic_cast<MirrorSphere *>(obj.get())) {
          // Exemplo: MirrorSphere->center.y += sin(time + j) / 150;
          mirrorSphere->center.z += sin(time) / 10;
        }
        // (Adicione lógica para Cylinder se tiver)
      }

      // Animação da Malha (usando o ponteiro que guardamos)
      if (bunnyMesh) {
        Point center = bunnyMesh->getCentroid();

        // Pivot: Origem -> Gira -> Volta
        Matrix4 toOrigin = Matrix4::translate(-center.x, -center.y, -center.z);
        Matrix4 rotation = Matrix4::rotateY(0.05f); // Velocidade da rotação
        Matrix4 fromOrigin = Matrix4::translate(center.x, center.y, center.z);

        Matrix4 pivotRotation = fromOrigin * rotation * toOrigin;
        bunnyMesh->applyTransform(pivotRotation);
      }

      // Luz
      lightPosition.y -= 0.001f;
      lightPosition.x += cos(time / 4) / 100;

      // --- 2. RENDERIZAÇÃO (PARALELO) ---
      std::vector<Color> frameBuffer(numCols * numRows);

// O OpenMP divide este loop entre os núcleos da CPU
#pragma omp parallel for schedule(dynamic)
      for (int l = 0; l < numRows; l++) {
        for (int c = 0; c < numCols; c++) {
          float x, y;
          convertDisplayToWindow(c, l, x, y);
          Vector4 d(x - observerPosition.x, y - observerPosition.y,
                    -viewplaneDistance, 0);

          Ray ray(d.normalize(), observerPosition);

          float closest_t;
          // getIntersectedObject usa AABB se for Mesh, então é rápido!
          Object *closestObject = getIntersectedObject(ray, objects, closest_t);

          if (closestObject == nullptr) {
            // CORREÇÃO: Escreve no buffer, NÃO no arquivo (evita conflito de
            // threads)
            frameBuffer[l * numCols + c] = Color(10, 50, 200);
          } else {
            Point P = ray.origin + (ray.dir * closest_t);

            Color finalColor = closestObject->shade(
                ray, P, lightPosition, lightIntensity, ambientLightIntensity,
                observerPosition, objects);

            finalColor.clamp();

            // Escreve no buffer
            frameBuffer[l * numCols + c] = finalColor;
          }
        }
      }

      // --- 3. ESCRITA NO ARQUIVO (SERIAL) ---
      // Agora que o buffer está pronto, escrevemos tudo de uma vez
      for (int l = 0; l < numRows; l++) {
        for (int c = 0; c < numCols; c++) {
          Color &cor = frameBuffer[l * numCols + c];
          image << cor.r << " " << cor.g << " " << cor.b << " ";
        }
        image << "\n";
      }

      image.close();
    }
  }
  std::cout << "Concluido!\n";
  return 0;
}
