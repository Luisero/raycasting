#ifndef MATRIX4_HPP
#define MATRIX4_HPP

#include "Point.hpp"
#include "Vector.hpp"
#include <cmath>

class Matrix4 {
public:
  // Armazena as 4 colunas da matriz
  Vector4 cols[4];

  Matrix4(); // Inicializa como Identidade

  // Multiplicação Matriz * Vetor (Transforma um ponto/vetor)
  Vector4 operator*(const Vector4 &v) const;

  // Multiplicação Matriz * Matriz (Combina transformações)
  Matrix4 operator*(const Matrix4 &m) const;

  // --- Fábrica de Matrizes de Transformação ---

  static Matrix4 identity();
  static Matrix4 translate(float x, float y, float z);
  static Matrix4 scale(float x, float y, float z);
  static Matrix4 rotateX(float angleRadians);
  static Matrix4 rotateY(float angleRadians);
  static Matrix4 rotateZ(float angleRadians);
};

#endif
