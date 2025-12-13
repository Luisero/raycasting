#include "../include/Mesh.hpp"
#include <algorithm> // Para std::swap, std::max, std::min
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

Mesh::Mesh() {
  // Inicializa a caixa com valores "invertidos" para forçar atualização
  float maxFloat = std::numeric_limits<float>::max();
  float minFloat = std::numeric_limits<float>::lowest();
  minBound = Point(maxFloat, maxFloat, maxFloat, 1);
  maxBound = Point(minFloat, minFloat, minFloat, 1);
}

// Implementação fictícia de getNormal (quem responde a normal é o triângulo
// interno)
Vector4 Mesh::getNormal(Point collide) { return Vector4(0, 1, 0, 0); }

// --- O SEGREDO DA VELOCIDADE: Teste de Interseção AABB (Slab Method) ---
bool Mesh::intersectAABB(Ray ray, float &tMinOut, float &tMaxOut) {
  // Testa os planos X
  float tx1 = (minBound.x - ray.origin.x) / ray.dir.x;
  float tx2 = (maxBound.x - ray.origin.x) / ray.dir.x;

  float tmin = std::min(tx1, tx2);
  float tmax = std::max(tx1, tx2);

  // Testa os planos Y
  float ty1 = (minBound.y - ray.origin.y) / ray.dir.y;
  float ty2 = (maxBound.y - ray.origin.y) / ray.dir.y;

  tmin = std::max(tmin, std::min(ty1, ty2));
  tmax = std::min(tmax, std::max(ty1, ty2));

  // Testa os planos Z
  float tz1 = (minBound.z - ray.origin.z) / ray.dir.z;
  float tz2 = (maxBound.z - ray.origin.z) / ray.dir.z;

  tmin = std::max(tmin, std::min(tz1, tz2));
  tmax = std::min(tmax, std::max(tz1, tz2));

  tMinOut = tmin;
  tMaxOut = tmax;

  // Se tmax < tmin, o raio errou a caixa
  return tmax >= tmin && tmax >= 0;
}

float Mesh::intersect(Ray ray) {
  float boxTMin, boxTMax;

  // 1. Pergunta para a caixa: "O raio passou perto?"
  if (!intersectAABB(ray, boxTMin, boxTMax)) {
    return -1.0f; // Nem perde tempo olhando os triângulos!
  }

  // 2. Se passou pela caixa, testa os triângulos um a um
  float closest_t = -1.0f;

  for (const auto &tri : triangles) {
    float t = tri->intersect(ray);
    if (t > 0.001f) {
      if (closest_t < 0 || t < closest_t) {
        closest_t = t;
      }
    }
  }
  return closest_t;
}

// --- SHADE: Repassa para o triângulo certo ---
Color Mesh::shade(const Ray &viewingRay, const Point &P,
                  const Point &lightPosition, const Color &lightIntensity,
                  const Color &ambientLightIntensity,
                  const Point &observerPosition,
                  const std::vector<std::unique_ptr<Object>> &allObjects) {

  // Precisamos descobrir QUAL triângulo foi atingido para pegar a normal dele.
  // Como intersect() só retorna float, refazemos o teste rápido aqui (seguro
  // para Threads)

  float closest_t = -1.0f;
  Triangle *hitTri = nullptr;

  // Otimização: Sabemos que P está no triângulo, então t deve ser a distância
  // até P
  float distToP = (P - viewingRay.origin).lenght();

  for (const auto &tri : triangles) {
    float t = tri->intersect(viewingRay);

    if (t > 0.001f && std::abs(t - distToP) < 0.001f) {
      hitTri = tri.get();
      break;
    }
  }

  if (hitTri) {

    return hitTri->shade(viewingRay, P, lightPosition, lightIntensity,
                         ambientLightIntensity, observerPosition, allObjects);
  }

  return Color(0, 0, 0); // Erro de segurança
}

bool Mesh::loadOBJ(const std::string &filename, Material mat) {
  std::ifstream file(filename);
  if (!file.is_open())
    return false;

  triangles.clear(); // Limpa triângulos antigos
  uvs.clear();
  this->vertices.clear();
  this->modelVertices.clear();
  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      float x, y, z;
      ss >> x >> y >> z;
      //      temp_verts.push_back(Point(x, y, z, 1.0f));
      this->vertices.push_back(Point(x, y, z, 1.0f));
      this->modelVertices.push_back(Point(x, y, z, 1.0f));
    } else if (prefix == "vt") {
      float u, v;
      ss >> u >> v;
      this->uvs.push_back(Vector2(u, v));
    } else if (prefix == "f") {
      std::string s1, s2, s3;
      ss >> s1 >> s2 >> s3;

      // --- NOVA LÓGICA DE PARSER ---
      // Função Lambda para extrair (Vértice) E (Textura) da string "v/vt/vn"
      auto parseToken = [](const std::string &token, int &vIdx, int &vtIdx) {
        size_t firstSlash = token.find('/');
        size_t secondSlash = token.find('/', firstSlash + 1);

        // Pega o Vértice (Sempre antes da primeira barra)
        if (firstSlash == std::string::npos) {
          vIdx = std::stoi(token); // Caso simples: "f 1"
          vtIdx = 0;               // Sem textura
          return;
        }
        vIdx = std::stoi(token.substr(0, firstSlash));

        // Pega a Textura (Entre a primeira e a segunda barra)
        // Formatos possíveis: "1/5" ou "1/5/3"
        if (firstSlash != std::string::npos) {
          // Verifica se tem algo entre as barras (para evitar caso "1//3")
          bool hasTexture = (secondSlash == std::string::npos) ||
                            (secondSlash > firstSlash + 1);

          if (hasTexture) {
            // Se não tiver segunda barra, pega até o fim. Se tiver, pega até
            // ela.
            size_t len = (secondSlash == std::string::npos)
                             ? std::string::npos
                             : (secondSlash - firstSlash - 1);
            vtIdx = std::stoi(token.substr(firstSlash + 1, len));
          } else {
            vtIdx = 0; // Caso "1//3" (Vértice//Normal)
          }
        } else {
          vtIdx = 0;
        }
      };

      int v1, vt1, v2, vt2, v3, vt3;

      // Extrai os índices para os 3 pontos do triângulo
      parseToken(s1, v1, vt1);
      parseToken(s2, v2, vt2);
      parseToken(s3, v3, vt3);

      // Validação de segurança (se os índices existem nos vetores)
      if (v1 > 0 && v1 <= (int)this->vertices.size() && v2 > 0 &&
          v2 <= (int)this->vertices.size() && v3 > 0 &&
          v3 <= (int)this->vertices.size()) {

        // 1. Pega os Vértices (Geometria)
        Point *p1 = &this->modelVertices[v1 - 1];
        Point *p2 = &this->modelVertices[v2 - 1];
        Point *p3 = &this->modelVertices[v3 - 1];

        // 2. Pega as UVs (Textura)
        // Se vt > 0, busca no vetor. Se for 0, usa padrão (0,0).
        Vector2 t1 = (vt1 > 0 && vt1 <= this->uvs.size()) ? this->uvs[vt1 - 1]
                                                          : Vector2(0, 0);
        Vector2 t2 = (vt2 > 0 && vt2 <= this->uvs.size()) ? this->uvs[vt2 - 1]
                                                          : Vector2(0, 0);
        Vector2 t3 = (vt3 > 0 && vt3 <= this->uvs.size()) ? this->uvs[vt3 - 1]
                                                          : Vector2(0, 0);

        // Cria o triângulo passando TUDO
        // (Você precisa atualizar o construtor do Triangle para aceitar t1, t2,
        // t3)
        triangles.push_back(
            std::make_unique<Triangle>(p1, p2, p3, &t1, &t2, &t3, mat));
      }
    }
  }

  std::cout << "Loaded " << triangles.size() << " triangles.\n";
  calculateBounds();
  return true;
}

void Mesh::calculateBounds() {
  if (triangles.empty())
    return;

  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float minZ = std::numeric_limits<float>::max();

  float maxX = std::numeric_limits<float>::lowest();
  float maxY = std::numeric_limits<float>::lowest();
  float maxZ = std::numeric_limits<float>::lowest();

  for (const auto &tri : triangles) {
    // Testa os 3 vértices de cada triângulo
    const Point *pts[] = {tri->v0, tri->v1, tri->v2};
    for (auto p : pts) {
      if (p->x < minX)
        minX = p->x;
      if (p->x > maxX)
        maxX = p->x;
      if (p->y < minY)
        minY = p->y;
      if (p->y > maxY)
        maxY = p->y;
      if (p->z < minZ)
        minZ = p->z;
      if (p->z > maxZ)
        maxZ = p->z;
    }
  }
  // Adiciona uma margem de segurança (epsilon) para evitar caixas com espessura
  // zero
  minBound = Point(minX - 0.01f, minY - 0.01f, minZ - 0.01f, 1.0f);
  maxBound = Point(maxX + 0.01f, maxY + 0.01f, maxZ + 0.01f, 1.0f);
}

void Mesh::applyTransform(const Matrix4 &t) {
  for (auto &tri : triangles) {
    tri->applyTransform(t);
  }
  calculateBounds(); // Recalcula a caixa se a malha se mexer!
}

Point Mesh::getCentroid() const {
  return Point((minBound.x + maxBound.x) / 2.0f,
               (minBound.y + maxBound.y) / 2.0f,
               (minBound.z + maxBound.z) / 2.0f, 1.0f);
}
