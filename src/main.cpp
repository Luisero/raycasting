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
#include <memory.h>
#include <memory>
#include <vector>

// --- CONSTANTS ---
const float windowWidth = 2.f, windowHeight = 1.5f;
const int numCols = windowWidth * 400, numRows = windowHeight * 400;
float Dx = windowWidth / numCols;
float Dy = windowHeight / numRows;
float viewplaneDistance = 10;
const int FRAMES_AMOUNT = 30;

Point observerPosition(0, 0, 0, 1);

Point lightPosition(-1.f, 2.0f, -viewplaneDistance / 2, 1.0f);
Color lightIntensity(255, 255, 255);
Color ambientLightIntensity(80, 80, 80);

void convertDisplayToWindow(int display_x, int display_y, float &ndc_x,
                            float &ndc_y) {
  // Adds 0.5 to get the center of the pixel
  ndc_x = -windowWidth / 2.0f + Dx / 2.0f + display_x * Dx;
  ndc_y = windowHeight / 2.0f - Dy / 2.0f - display_y * Dy;
}

// --- A FUNÇÃO getIntersectedObject FOI MOVIDA PARA Object.cpp ---
// (Ela ainda é declarada em Object.hpp, que nós incluímos)

int main() {
  float time = 0;
  // --- MATERIALS ---
  Material matOrange(Color(10, 5, 2), Color(200, 100, 50), Color(255, 255, 255),
                     128.0f);
  Material matPink(Color(10, 5, 2), Color(255, 141, 161), Color(255, 255, 255),
                   128.0f);
  Material matRed(Color(10, 5, 2), Color(255, 10, 20), Color(255, 255, 255),
                  128.0f);
  Material matFloor(Color(40, 90, 50), Color(35, 196, 12), Color(255, 255, 255),
                    255.f);
  Material matWall(Color(40, 40, 50),    //
                   Color(35, 100, 196),  //
                   Color(255, 255, 255), //
                   99999.f);             //
  Material matMirror(Color(0, 0, 0), Color(0, 0, 0), Color(0, 0, 0), 128.f);

  float sphereRadius = .2f;
  Point defaultCenter(0.f, 0.f, -viewplaneDistance + sphereRadius, 1.f);

  std::vector<std::unique_ptr<Object>> objects;

  // --- SCENE OBJECTS ---
  // (Esta seção não muda)
  objects.push_back(std::make_unique<Sphere>(
      defaultCenter + Vector4(-0.7f, 0, -2, 0), sphereRadius, matOrange));
  objects.push_back(std::make_unique<Sphere>(
      defaultCenter + Vector4(-0.8f, 0.4f, 1.0f, 0.f), sphereRadius, matPink));

  Mesh cubeMesh; // Instância da nossa malha

  Material matCube(
      Color(28, 14, 4), // Ka (Ambiente: um marrom quase preto para as sombras)
      Color(139, 69, 19),   // Kd (Difuso: a cor principal marrom "SaddleBrown")
      Color(255, 255, 255), // Ks (Especular: brilho branco)
      64.0f                 // Shininess (Concentração do brilho)
  );
  // 1. Carrega o objeto (ele vai aparecer na origem 0,0,0 gigante)
  cubeMesh.loadOBJ("cube.obj", objects, matCube);
  Matrix4 setupMatrix = Matrix4::translate(0.5f, 0.5f, -viewplaneDistance) *
                        Matrix4::scale(0.5f, 0.5f, 0.5f) * Matrix4::rotateX(3);
  cubeMesh.applyTransform(setupMatrix);
  Point cylinderBase = Point(-.5f, -sphereRadius, -viewplaneDistance, 1.0f);
  Vector4 cylinderAxis =
      Vector4(0.0f, 1.0f, 0.0f, 0.0f); // Apontando ao longo do eixo X
  float cylinderRadius = 0.1f;
  float cylinderHeight = 2.0f;

  objects.push_back(std::make_unique<Cylinder>(cylinderBase, cylinderAxis,
                                               cylinderRadius, cylinderHeight,
                                               matPink // Usando matFloor
                                               ));

  objects.push_back(std::make_unique<MirrorSphere>(
      Point(0, 0.f, -viewplaneDistance, 1), sphereRadius, matRed));

  objects.push_back(std::make_unique<Triangle>(
      Point(-0.1f, 0.5f, -viewplaneDistance, 0),
      Point(0.5f, 0.5f, -viewplaneDistance, 0),
      Point(0, 1.f, -viewplaneDistance, 0), matOrange));

  Point floorPoint(0.f, -sphereRadius * 2.f, 0.f, 1.f);
  Vector4 floorNormal(0.f, 1.f, 0.f, 0.f);
  objects.push_back(std::make_unique<Plane>(floorPoint, floorNormal, matFloor));
  Point wallPoint(0.f, 0.f, -viewplaneDistance * 2, 1.f);
  Vector4 wallNormal(0, 0, 1, 0);
  objects.push_back(std::make_unique<Plane>(wallPoint, wallNormal, matWall));

  // --- MAIN LOOP ---
  for (int i = 0; i < FRAMES_AMOUNT; i++) {
    // system("clear");
    std::cout << "Rendering [ " << i << " ] frame.\n";
    time += 0.1;

    std::string frametitle = "image";
    frametitle.append(std::to_string(i));
    frametitle.append(".ppm");
    std::ofstream image(frametitle);

    if (image.is_open()) {
      image << "P3\n";
      image << numCols << " " << numRows << "\n";
      image << 255 << "\n";

      // --- Animation Step ---
      // (Esta seção não muda)
      for (int j = 0; j < objects.size(); j++) {
        if (objects[j] == nullptr) {
          continue;
        }
        if (Sphere *sphere = dynamic_cast<Sphere *>(objects[j].get())) {
          sphere->center.y += sin(time + j) / 150;
          sphere->center.z += cos(time + j) / 10;
        }
        if (Cylinder *cylinder = dynamic_cast<Cylinder *>(objects[j].get())) {
          cylinder->axis.x += sin(time) / 100;
          cylinder->axis.z += cos(time) / 100;
          cylinder->axis.y += sin(time) / 100;
          cylinder->center.x -= sin(time) / 100;
        }

        // 1. Descobre onde está o centro ATUAL da malha
        Point center = cubeMesh.getCentroid();

        // 2. Cria as 3 matrizes do "Pivot"

        // A: Move o centro para a origem (0,0,0)
        Matrix4 toOrigin = Matrix4::translate(-center.x, -center.y, -center.z);

        // B: Faz a rotação desejada (no eixo Y, por exemplo)
        Matrix4 rotation = Matrix4::rotateY(0.01f); // Gira um pouquinho

        // C: Move de volta para a posição original
        Matrix4 fromOrigin = Matrix4::translate(center.x, center.y, center.z);

        // 3. Combina tudo: T_volta * R * T_ida
        //    (Lembre-se: a ordem de aplicação é da direita para a esquerda)
        Matrix4 pivotRotation = fromOrigin * rotation * toOrigin;
        cubeMesh.applyTransform(pivotRotation);
      }
      lightPosition.y -= 0.001f;
      lightPosition.x += cos(time / 4) / 100;

      // --- Render Step ---
      for (int l = 0; l < numRows; l++) {
        for (int c = 0; c < numCols; c++) {
          float x, y;
          convertDisplayToWindow(c, l, x, y);
          Vector4 d(x - observerPosition.x, y - observerPosition.y,
                    -viewplaneDistance, 0);

          Ray ray(d.normalize(), observerPosition);

          float closest_t;
          Object *closestObject = getIntersectedObject(ray, objects, closest_t);

          if (closestObject == nullptr) {
            image << "10 50 200 "; // Background color
          } else {
            // --- LÓGICA DE SOMBREAMENTO SIMPLIFICADA ---

            // 1. Encontre o ponto de colisão
            Point P = ray.origin + (ray.dir * closest_t);

            // 2. Peça ao objeto para calcular sua própria cor
            //    O objeto vai decidir se faz Phong, Reflexão, etc.
            Color finalColor = closestObject->shade(
                ray, P, lightPosition, lightIntensity, ambientLightIntensity,
                observerPosition, objects);

            // 3. Escreva a cor no arquivo
            finalColor.clamp();
            image << finalColor.r << " " << finalColor.g << " " << finalColor.b
                  << " ";
          }
        }
        image << "\n";
      }
      image.close();
    }
  }
  return 0;
}
