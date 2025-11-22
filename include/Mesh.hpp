#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <memory>
#include "Object.hpp"   // Herda de Object
#include "Triangle.hpp" // Guarda Triangles
#include "Matrix4.hpp"
#include "Point.hpp"

// A Mesh agora é um Object da cena!
class Mesh : public Object {
public:
    // Os triângulos ficam guardados aqui dentro (privados ao mundo, públicos para a Mesh)
    std::vector<std::unique_ptr<Triangle>> triangles;
    
    // A Caixa Envolvente (AABB) definida por dois pontos extremos
    Point minBound;
    Point maxBound;

    Mesh(); // Construtor

    // --- Métodos Obrigatórios de Object ---
    virtual float intersect(Ray ray) override;
    virtual Vector4 getNormal(Point collide) override; // (Não usado diretamente, mas obrigatório)
    virtual Color shade(const Ray& viewingRay, const Point& intersectionPoint, const Point& lightPosition, const Color& lightIntensity, const Color& ambientLightIntensity, const Point& observerPosition, const std::vector<std::unique_ptr<Object>>& allObjects) override;

    // --- Métodos da Malha ---
    bool loadOBJ(const std::string& filename, Material mat);
    void applyTransform(const Matrix4& transform);
    Point getCentroid() const;

private:
    // Métodos internos para gerenciar a caixa
    void calculateBounds();
    bool intersectAABB(Ray ray, float& tMinOut, float& tMaxOut);
};

#endif