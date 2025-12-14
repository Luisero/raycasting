#include "../include/Triangle.hpp"
#include "../include/Object.hpp" // Para getIntersectedObject
#include "../include/Texture.hpp"
#include <algorithm>
#include <cmath>

Triangle::Triangle(Point *a, Point *b, Point *c, Vector2 *t1, Vector2 *t2,
                   Vector2 *t3, Material mat)
{
  this->v0 = a;
  this->v1 = b;
  this->v2 = c;
  this->t1 = t1;
  this->t2 = t2;
  this->t3 = t3;
  this->material = mat;

  // --- Pré-calcula a Normal ---
  // Aresta 1: v1 - v0
  // Aresta 2: v2 - v0
  // Normal = Aresta1 X Aresta2 (Produto Vetorial)
  Vector4 edge1 = *v1 - *v0;
  Vector4 edge2 = *v2 - *v0;
  this->normal = edge1.cross(edge2).normalized();

  // Define um centro aproximado (baricentro) para a classe base, se necessário
  this->center = Point((v0->x + v1->x + v2->x) / 3, (v0->y + v1->y + v2->y) / 3,
                       (v0->z + v1->z + v2->z) / 3, 1.0f);
}

Vector4 Triangle::getNormal(Point collide)
{
  // Para um triângulo plano, a normal é constante em toda a face
  return this->normal;
}

// --- Algoritmo Möller–Trumbore ---
float Triangle::intersect(Ray ray)
{
  //  if (ray.dir.dot(this->normal) >= 0)
  //  return -1;
  const float EPSILON = 0.0000001f;
  // const float EPSILON = 0.0f;

  Vector4 edge1 = *v1 - *v0;
  Vector4 edge2 = *v2 - *v0;

  Vector4 h = ray.dir.cross(edge2);
  float a = edge1.dot(h);

  // Se 'a' for próximo de 0, o raio é paralelo ao triângulo
  if (a > -EPSILON && a < EPSILON)
  {
    return -1.0f;
  }

  float f = 1.0f / a;
  Vector4 s = ray.origin - *v0;
  float u = f * s.dot(h);

  // Verifica se a interseção está fora do triângulo (coord. baricêntrica u)
  if (u < 0.0f || u > 1.0f)
  {
    return -1.0f;
  }

  Vector4 q = s.cross(edge1);
  float v = f * ray.dir.dot(q);

  // Verifica se a interseção está fora do triângulo (coord. baricêntrica v)
  if (v < 0.0f || u + v > 1.0f)
  {
    return -1.0f;
  }

  // Neste ponto, temos uma linha de interseção. Vamos descobrir o 't'.
  float t = f * edge2.dot(q);

  if (t > EPSILON)
  { // Interseção válida
    return t;
  }
  else
  { // Interseção atrás da origem do raio
    return -1.0f;
  }
}

// Copie a função shade do Plane.cpp ou Sphere.cpp, a lógica Phong é idêntica
Color Triangle::shade(const Ray &viewingRay, const Point &P,
                      const Point &lightPosition, const Color &lightIntensity,
                      const Color &ambientLightIntensity,
                      const Point &observerPosition,
                      const std::vector<std::unique_ptr<Object>> &allObjects)
{

  Material mat = this->material;
  Vector4 N = this->getNormal(P);
  N.normalize();

  // 1. Normal Flip: Garante que a face esteja virada para a luz
  if (viewingRay.dir.dot(N) > 0)
  {
    N = N * -1.0f;
  }

  // --- PASSO 1: DESCOBRIR A COR BASE (Sólida ou Textura) ---
  // Fazemos isso ANTES de calcular qualquer luz!
  Color objectColor = mat.Kd; // Começa com a cor padrão do material

  if (mat.texture != nullptr)
  {

    Vector4 edge1 = *v1 - *v0;
    Vector4 edge2 = *v2 - *v0;
    Vector4 v0p = P - *v0;

    float d00 = edge1.dot(edge1);
    float d01 = edge1.dot(edge2);
    float d11 = edge2.dot(edge2);
    float d20 = v0p.dot(edge1);
    float d21 = v0p.dot(edge2);
    float denom = d00 * d11 - d01 * d01;

    if (std::abs(denom) > 1e-15)
    {
      float v = (d11 * d20 - d01 * d21) / denom;
      float w = (d00 * d21 - d01 * d20) / denom;
      float u = 1.0f - v - w;

      // Interpolação UV (Baricêntrica)
      float texU = u * t1->u + v * t2->u + w * t3->u;
      float texV = u * t1->v + v * t2->v + w * t3->v;
      if (texU > 1 || texU < 0 || texV > 1 || texV < 0)
        std::cout << "erro";
      //texU = std::clamp(texU, 0.0f, 1.0f);
      //texV = std::clamp(texV, 0.0f, 1.0f);

      // texV = 1.0f - texV;
      float debugU = texU - std::floor(texU);
      float debugV = texV - std::floor(texV);

      // return Color(debugU * 255, debugV * 255, 0);
      //  Atualiza a cor base com o pixel da imagem
      objectColor = mat.texture->getColor(texU, texV);
    
    
    }
  }

  // --- PASSO 2: CALCULAR LUZES (Usando a cor descoberta) ---

  // Ambiente: 10% da cor da textura
  Color ambientColor = objectColor * 0.2f;

  Color diffuseColor(0, 0, 0);
  Color specularColor(0, 0, 0);

  Vector4 lightVector = (lightPosition - P);
  float distanceToLight = lightVector.lenght();
  Vector4 lightDirection = lightVector.normalized();

  // Shadow Check
  Point shadowRayOrigin = P + (N * 0.00001f);
  Ray shadowRay(lightDirection, shadowRayOrigin);
  float shadow_t;
  Object *obstructingObject = getIntersectedObject(shadowRay, allObjects, shadow_t);
  bool inShadow = (obstructingObject != nullptr) && (shadow_t < distanceToLight);

  if (!inShadow)
  {
    // Difusa
    float diffuseFactor = std::max(0.0f, N.dot(lightDirection));
    if (diffuseFactor > 0)
    {
      // CORREÇÃO: Usamos objectColor (Textura) * Luz. Não use mat.Kd aqui.
      diffuseColor = (objectColor * lightIntensity) * diffuseFactor;
    }

    // Especular (Brilho branco)
    Vector4 V = (observerPosition - P).normalized();
    Vector4 R = (N * (2.0f * N.dot(lightDirection))) - lightDirection;
    float specularFactor = std::pow(std::max(0.0f, V.dot(R.normalized())), mat.shininess);

    if (specularFactor > 0)
    {
      // O brilho especular continua usando Ks (geralmente branco)
      specularColor = (mat.Ks * lightIntensity) * specularFactor;
    }
  }

  // --- PASSO 3: SOMA FINAL ---
  // CORREÇÃO CRÍTICA: Removi o "+ objectColor" extra do final.
  // A cor já está dentro de 'ambient' e 'diffuse'. Somar ela de novo causava o branco excessivo.
  
  return ambientColor + diffuseColor + specularColor;
}
void Triangle::applyTransform(const Matrix4 &m)
{
  // 1. Transforma os 3 vértices
  // O operador * (Matriz * Vetor) já lida com Point corretamente se w=1
  Vector4 newV0 = m * Vector4(v0->x, v0->y, v0->z, 1.0f);
  Vector4 newV1 = m * Vector4(v1->x, v1->y, v1->z, 1.0f);
  Vector4 newV2 = m * Vector4(v2->x, v2->y, v2->z, 1.0f);

  v0 = new Point(newV0.x, newV0.y, newV0.z, 1.0f);
  v1 = new Point(newV1.x, newV1.y, newV1.z, 1.0f);
  v2 = new Point(newV2.x, newV2.y, newV2.z, 1.0f);

  // 2. Recalcula a Normal (a rotação pode ter mudado a orientação)
  Vector4 edge1 = *v1 - *v0;
  Vector4 edge2 = *v2 - *v0;
  this->normal = edge1.cross(edge2).normalized();

  // 3. Recalcula o Centro (para animações genéricas que usem obj->center)
  this->center =
      Point((v0->x + v1->x + v2->x) / 3.0f, (v0->y + v1->y + v2->y) / 3.0f,
            (v0->z + v1->z + v2->z) / 3.0f, 1.0f);
}

void Triangle::recalculateNormal()
{
  // Recalcula a normal baseada nas novas posições de v0, v1, v2
  Vector4 edge1 = *v1 - *v0;
  Vector4 edge2 = *v2 - *v0;
  this->normal = edge1.cross(edge2).normalized();
  // Opcional: Recalcular o centro também, se usar BVH
}