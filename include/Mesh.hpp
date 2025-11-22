#ifndef MESH_HPP
#define MESH_HPP

#include "Material.hpp"
#include "Matrix4.hpp"
#include "Object.hpp"
#include "Point.hpp"
#include "Triangle.hpp" // Precisamos conhecer o Triangle
#include <memory>
#include <string>
#include <vector>

class Mesh {
public:
  // Guarda ponteiros para os triângulos que pertencem a esta malha.
  // (Nota: O 'main' continua sendo o dono da memória via unique_ptr,
  //  aqui só guardamos uma referência para acesso rápido)
  std::vector<Triangle *> myTriangles;
  Point getCentroid() const;
  // Carrega o OBJ e cria os triângulos
  bool loadOBJ(const std::string &filename,
               std::vector<std::unique_ptr<Object>> &sceneObjects,
               Material mat);

  // --- NOVO: Transforma a Malha Inteira ---
  // Percorre 'myTriangles' e aplica a matriz em cada um
  void applyTransform(const Matrix4 &transform);
};

#endif
