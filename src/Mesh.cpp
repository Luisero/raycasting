#include "../include/Mesh.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Função Load simplificada (sem transform inicial, pois faremos isso depois)
bool Mesh::loadOBJ(const std::string &filename,
                   std::vector<std::unique_ptr<Object>> &sceneObjects,
                   Material mat) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Erro ao abrir: " << filename << std::endl;
    return false;
  }

  std::vector<Point> temp_vertices;
  std::string line;

  // Limpa triângulos antigos se estiver recarregando
  myTriangles.clear();

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      float x, y, z;
      ss >> x >> y >> z;
      temp_vertices.push_back(Point(x, y, z, 1.0f));
    } else if (prefix == "f") {
      // Lê as três partes da face como strings primeiro
      std::string s1, s2, s3;
      ss >> s1 >> s2 >> s3;

      // Função auxiliar (Lambda) para pegar apenas o primeiro número antes da
      // barra
      auto getIndex = [](const std::string &s) -> int {
        size_t firstSlash = s.find('/');
        if (firstSlash == std::string::npos) {
          return std::stoi(s); // Não tem barra, é só o número
        }
        return std::stoi(s.substr(0, firstSlash)); // Pega o texto antes da '/'
      };

      // Converte as strings para índices inteiros
      int i1 = getIndex(s1);
      int i2 = getIndex(s2);
      int i3 = getIndex(s3);

      // Validamos os índices (OBJ começa em 1)
      if (i1 > 0 && i2 > 0 && i3 > 0 && i1 <= (int)temp_vertices.size() &&
          i2 <= (int)temp_vertices.size() && i3 <= (int)temp_vertices.size()) {

        Point p1 = temp_vertices[i1 - 1];
        Point p2 = temp_vertices[i2 - 1];
        Point p3 = temp_vertices[i3 - 1];

        // Cria e guarda o triângulo
        auto tri = std::make_unique<Triangle>(p1, p2, p3, mat);
        myTriangles.push_back(tri.get());
        sceneObjects.push_back(std::move(tri));
      }
    }
  }
  std::cout << "Mesh carregada: " << filename << " com " << myTriangles.size()
            << " triangulos." << std::endl;
  return true;
}

// Implementação da Transformação em Grupo
void Mesh::applyTransform(const Matrix4 &transform) {
  for (Triangle *tri : myTriangles) {
    tri->applyTransform(transform);
  }
}

Point Mesh::getCentroid() const {
  if (myTriangles.empty()) {
    return Point(0, 0, 0, 1);
  }

  // Inicializa min/max com valores extremos
  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float minZ = std::numeric_limits<float>::max();

  float maxX = std::numeric_limits<float>::lowest();
  float maxY = std::numeric_limits<float>::lowest();
  float maxZ = std::numeric_limits<float>::lowest();

  // Percorre todos os triângulos para achar os limites da malha
  for (const Triangle *tri : myTriangles) {
    // Verifica os 3 vértices de cada triângulo
    const Point *pts[3] = {&tri->v0, &tri->v1, &tri->v2};

    for (int i = 0; i < 3; i++) {
      if (pts[i]->x < minX)
        minX = pts[i]->x;
      if (pts[i]->x > maxX)
        maxX = pts[i]->x;

      if (pts[i]->y < minY)
        minY = pts[i]->y;
      if (pts[i]->y > maxY)
        maxY = pts[i]->y;

      if (pts[i]->z < minZ)
        minZ = pts[i]->z;
      if (pts[i]->z > maxZ)
        maxZ = pts[i]->z;
    }
  }

  // O centro é a média dos limites (centro da caixa)
  return Point((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f,
               1.0f);
}
