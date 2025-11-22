#include "../include/Matrix4.hpp"

// O Construtor agora inicializa a Identidade diretamente
Matrix4::Matrix4() {
  // Coluna 0: (1, 0, 0, 0)
  cols[0] = Vector4(1, 0, 0, 0);
  // Coluna 1: (0, 1, 0, 0)
  cols[1] = Vector4(0, 1, 0, 0);
  // Coluna 2: (0, 0, 1, 0)
  cols[2] = Vector4(0, 0, 1, 0);
  // Coluna 3: (0, 0, 0, 1)
  cols[3] = Vector4(0, 0, 0, 1);
}

Matrix4 Matrix4::identity() {
  // Como o construtor padrão já cria uma identidade,
  // basta retornar uma nova matriz!
  return Matrix4();
}

// Resto do arquivo continua igual...
Vector4 Matrix4::operator*(const Vector4 &v) const {
  Vector4 res;
  res = cols[0] * v.x + cols[1] * v.y + cols[2] * v.z + cols[3] * v.w;
  return res;
}

Matrix4 Matrix4::operator*(const Matrix4 &other) const {
  Matrix4 res;
  res.cols[0] = (*this) * other.cols[0];
  res.cols[1] = (*this) * other.cols[1];
  res.cols[2] = (*this) * other.cols[2];
  res.cols[3] = (*this) * other.cols[3];
  return res;
}

Matrix4 Matrix4::translate(float x, float y, float z) {
  Matrix4 m; // Já nasce identidade
  m.cols[3] = Vector4(x, y, z, 1.0f);
  return m;
}

Matrix4 Matrix4::scale(float x, float y, float z) {
  Matrix4 m; // Já nasce identidade
  m.cols[0].x = x;
  m.cols[1].y = y;
  m.cols[2].z = z;
  return m;
}

Matrix4 Matrix4::rotateX(float angle) {
  Matrix4 m; // Já nasce identidade
  float c = std::cos(angle);
  float s = std::sin(angle);

  m.cols[1].y = c;
  m.cols[1].z = s;
  m.cols[2].y = -s;
  m.cols[2].z = c;
  return m;
}

Matrix4 Matrix4::rotateY(float angle) {
  Matrix4 m; // Já nasce identidade
  float c = std::cos(angle);
  float s = std::sin(angle);

  m.cols[0].x = c;
  m.cols[0].z = -s;
  m.cols[2].x = s;
  m.cols[2].z = c;
  return m;
}

Matrix4 Matrix4::rotateZ(float angle) {
  Matrix4 m; // Já nasce identidade
  float c = std::cos(angle);
  float s = std::sin(angle);

  m.cols[0].x = c;
  m.cols[0].y = s;
  m.cols[1].x = -s;
  m.cols[1].y = c;
  return m;
}
