#include "../include/Mesh.hpp"
#include <algorithm> // Para std::swap, std::max, std::min, std::sort
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>

Mesh::Mesh() {
  // Inicializa a caixa com valores "invertidos" para forçar atualização
  float maxFloat = std::numeric_limits<float>::max();
  float minFloat = std::numeric_limits<float>::lowest();
  minBound = Point(maxFloat, maxFloat, maxFloat, 1);
  maxBound = Point(minFloat, minFloat, minFloat, 1);
}

// A normal é respondida pelo triângulo específico atingido, não pela malha
Vector4 Mesh::getNormal(Point collide) { return Vector4(0, 1, 0, 0); }

// --- TESTE AABB GENÉRICO (Slab Method) ---
// Verifica se o raio atinge uma caixa definida por minB e maxB
bool Mesh::testAABB(const Ray &ray, const Point &minB, const Point &maxB) {
  float tx1 = (minB.x - ray.origin.x) / ray.dir.x;
  float tx2 = (maxB.x - ray.origin.x) / ray.dir.x;
  float tmin = std::min(tx1, tx2);
  float tmax = std::max(tx1, tx2);

  float ty1 = (minB.y - ray.origin.y) / ray.dir.y;
  float ty2 = (maxB.y - ray.origin.y) / ray.dir.y;
  tmin = std::max(tmin, std::min(ty1, ty2));
  tmax = std::min(tmax, std::max(ty1, ty2));

  float tz1 = (minB.z - ray.origin.z) / ray.dir.z;
  float tz2 = (maxB.z - ray.origin.z) / ray.dir.z;
  tmin = std::max(tmin, std::min(tz1, tz2));
  tmax = std::min(tmax, std::max(tz1, tz2));

  return tmax >= tmin && tmax >= 0;
}

// --- CONSTRUÇÃO DO BVH ---
void Mesh::buildBVH() {
  if (triangles.empty())
    return;

  // Cria lista de ponteiros crus (Triangle*) para manipular na construção
  // (A memória real continua segura no std::vector<unique_ptr> triangles)
  std::vector<Triangle *> triPtrs;
  triPtrs.reserve(triangles.size());
  for (auto &t : triangles) {
    triPtrs.push_back(t.get());
  }

  // Inicia a construção recursiva
  bvhRoot = recursiveBuild(triPtrs);
  // std::cout << "BVH construido para " << triPtrs.size() << " triangulos.\n";
}

std::unique_ptr<BVHNode> Mesh::recursiveBuild(std::vector<Triangle *> &tris) {
  auto node = std::make_unique<BVHNode>();

  // 1. Calcula AABB deste nó (englobando todos os triângulos dele)
  float minX = std::numeric_limits<float>::max(),
        maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max(),
        maxY = std::numeric_limits<float>::lowest();
  float minZ = std::numeric_limits<float>::max(),
        maxZ = std::numeric_limits<float>::lowest();

  for (auto t : tris) {
    // Testa os 3 vértices de cada triângulo
    const Point *pts[] = {t->v0, t->v1, t->v2};
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
  // Margem de segurança (epsilon) para evitar caixas de espessura zero
  node->minBox = Point(minX - 0.001f, minY - 0.001f, minZ - 0.001f, 1);
  node->maxBox = Point(maxX + 0.001f, maxY + 0.001f, maxZ + 0.001f, 1);

  // 2. Condição de Parada (Folha)
  // Se tiver 4 ou menos triângulos, para de dividir e vira folha
  if (tris.size() <= 4) {
    node->leafTriangles = tris;
    return node;
  }

  // 3. Divisão (Split)
  // Descobre o eixo mais longo da caixa atual
  float sizeX = maxX - minX;
  float sizeY = maxY - minY;
  float sizeZ = maxZ - minZ;

  int axis = 0; // 0=X, 1=Y, 2=Z
  if (sizeY > sizeX && sizeY > sizeZ)
    axis = 1;
  else if (sizeZ > sizeX && sizeZ > sizeY)
    axis = 2;

  // Ordena os triângulos pelo seu centróide ao longo desse eixo
  auto centroidSorter = [axis](Triangle *a, Triangle *b) {
    Point ca = a->center;
    Point cb = b->center;
    if (axis == 0)
      return ca.x < cb.x;
    if (axis == 1)
      return ca.y < cb.y;
    return ca.z < cb.z;
  };

  std::sort(tris.begin(), tris.end(), centroidSorter);

  // Divide a lista ao meio
  size_t mid = tris.size() / 2;
  std::vector<Triangle *> leftTris(tris.begin(), tris.begin() + mid);
  std::vector<Triangle *> rightTris(tris.begin() + mid, tris.end());

  // Recursão
  node->left = recursiveBuild(leftTris);
  node->right = recursiveBuild(rightTris);

  return node;
}

// --- INTERSEÇÃO BVH (O(log N)) ---
bool Mesh::intersectBVH(BVHNode *node, const Ray &ray, float &closest_t,
                        Triangle *&hitTri) {
  // 1. O raio bate na caixa deste nó?
  if (!testAABB(ray, node->minBox, node->maxBox)) {
    return false;
  }

  // Se já achamos um objeto mais perto do que essa caixa inteira, não precisa
  // entrar (Isso requer calcular o tMin da caixa, o que simplifiquei aqui, mas
  // é uma otimização futura)

  bool hitSomething = false;

  // 2. Se for Folha, testa os triângulos linearmente
  if (node->isLeaf()) {
    for (auto tri : node->leafTriangles) {
      float t = tri->intersect(ray);
      if (t > 0.0001f) {
        if (closest_t < 0 || t < closest_t) {
          closest_t = t;
          hitTri = tri;
          hitSomething = true;
        }
      }
    }
    return hitSomething;
  }

  // 3. Se for Nó Interno, desce recursivamente nos filhos
  bool h1 = intersectBVH(node->left.get(), ray, closest_t, hitTri);
  bool h2 = intersectBVH(node->right.get(), ray, closest_t, hitTri);

  return h1 || h2;
}

// Substitui o intersect antigo linear pelo BVH rápido
float Mesh::intersect(Ray ray) {
  if (!bvhRoot)
    return -1.0f;

  float closest_t = -1.0f;
  Triangle *hitTri = nullptr;

  intersectBVH(bvhRoot.get(), ray, closest_t, hitTri);

  return closest_t;
}

// --- SHADE ---
Color Mesh::shade(const Ray &viewingRay, const Point &P,
                  const Point &lightPosition, const Color &lightIntensity,
                  const Color &ambientLightIntensity,
                  const Point &observerPosition,
                  const std::vector<std::unique_ptr<Object>> &allObjects) {

  // Usa o BVH para encontrar rapidamente qual triângulo contém o ponto P
  float closest_t = -1.0f;
  Triangle *hitTri = nullptr;

  float distToP = (P - viewingRay.origin).lenght();

  intersectBVH(bvhRoot.get(), viewingRay, closest_t, hitTri);

  if (hitTri) {
    // Pequena validação para garantir que pegamos o triângulo certo
    if (std::abs(closest_t - distToP) < 0.01f) {
      return hitTri->shade(viewingRay, P, lightPosition, lightIntensity,
                           ambientLightIntensity, observerPosition, allObjects);
    }
  }

  // Se o BVH falhou por precisão, tenta usar o triângulo encontrado
  if (hitTri)
    return hitTri->shade(viewingRay, P, lightPosition, lightIntensity,
                         ambientLightIntensity, observerPosition, allObjects);

  return Color(0, 0, 0);
}

// --- LOAD OBJ ---
bool Mesh::loadOBJ(const std::string &filename, Material mat) {
  std::ifstream file(filename);
  if (!file.is_open())
    return false;

  triangles.clear();
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
      this->vertices.push_back(Point(x, y, z, 1.0f));
      this->modelVertices.push_back(Point(x, y, z, 1.0f));
    } else if (prefix == "vt") {
      float u, v;
      ss >> u >> v;
      this->uvs.push_back(Vector2(u, v));
    } else if (prefix == "f") {
      std::vector<std::string> tokens;
      std::string token;

      while (ss >> token) {
        tokens.push_back(token);
      }

      auto parseToken = [](const std::string &token, int &vIdx, int &vtIdx) {
        size_t firstSlash = token.find('/');
        size_t secondSlash = token.find('/', firstSlash + 1);

        if (firstSlash == std::string::npos) {
          vIdx = std::stoi(token);
          vtIdx = 0;
          return;
        }
        vIdx = std::stoi(token.substr(0, firstSlash));

        if (firstSlash != std::string::npos) {
          bool hasTexture = (secondSlash == std::string::npos) ||
                            (secondSlash > firstSlash + 1);
          if (hasTexture) {
            size_t len = (secondSlash == std::string::npos)
                             ? std::string::npos
                             : (secondSlash - firstSlash - 1);
            vtIdx = std::stoi(token.substr(firstSlash + 1, len));
          } else {
            vtIdx = 0;
          }
        } else {
          vtIdx = 0;
        }
      };

      // Fan Triangulation
      for (size_t i = 2; i < tokens.size(); i++) {
        int v1, vt1, v2, vt2, v3, vt3;

        parseToken(tokens[0], v1, vt1);
        parseToken(tokens[i - 1], v2, vt2);
        parseToken(tokens[i], v3, vt3);

        if (v1 > 0 && v1 <= (int)this->vertices.size() && v2 > 0 &&
            v2 <= (int)this->vertices.size() && v3 > 0 &&
            v3 <= (int)this->vertices.size()) {

          Point *p1 = &this->vertices[v1 - 1];
          Point *p2 = &this->vertices[v2 - 1];
          Point *p3 = &this->vertices[v3 - 1];

          // UV padrão (meio da textura)
          static Vector2 defaultUV(0.5f, 0.5f);

          Vector2 *t1 = (vt1 > 0 && vt1 <= (int)this->uvs.size())
                            ? &this->uvs[vt1 - 1]
                            : &defaultUV;
          Vector2 *t2 = (vt2 > 0 && vt2 <= (int)this->uvs.size())
                            ? &this->uvs[vt2 - 1]
                            : &defaultUV;
          Vector2 *t3 = (vt3 > 0 && vt3 <= (int)this->uvs.size())
                            ? &this->uvs[vt3 - 1]
                            : &defaultUV;

          triangles.push_back(
              std::make_unique<Triangle>(p1, p2, p3, t1, t2, t3, mat));
        }
      }
    }
  }

  std::cout << "Loaded " << triangles.size() << " triangles. Building BVH...\n";
  calculateBounds(); // Atualiza limites globais
  buildBVH();        // Constrói a árvore de aceleração
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
  minBound = Point(minX - 0.01f, minY - 0.01f, minZ - 0.01f, 1.0f);
  maxBound = Point(maxX + 0.01f, maxY + 0.01f, maxZ + 0.01f, 1.0f);
}

void Mesh::applyTransform(const Matrix4 &m) {
  for (auto &v : this->vertices) {
    Vector4 newPos = m * Vector4(v.x, v.y, v.z, 1.0f);
    v.x = newPos.x;
    v.y = newPos.y;
    v.z = newPos.z;
  }

  // 2. Avisa os triângulos que os pontos mudaram
  // Eles precisam recalcular a Normal da face
  for (auto &tri : this->triangles) {
    tri->recalculateNormal();
  }
  calculateBounds(); // Recalcula a caixa se a malha se mexer!
  this->buildBVH();
}

Point Mesh::getCentroid() const {
  return Point((minBound.x + maxBound.x) / 2.0f,
               (minBound.y + maxBound.y) / 2.0f,
               (minBound.z + maxBound.z) / 2.0f, 1.0f);
}
