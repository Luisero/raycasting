#ifndef VECTOR4
#define VECTOR4

#include <cmath>
class Vector4 {
public:
  float x, y, z, w;
  Vector4(float x, float y, float z, float w);
  Vector4();

  // --- MUDANÇA: Adicionado 'const' no final ---
  float dot(Vector4 vec) const {
    float res = this->x * vec.x;
    res += this->y * vec.y;
    res += this->z * vec.z;
    res += this->w * vec.w;
    return res;
  };

  // --- MUDANÇA: Adicionado 'const' no final ---
  Vector4 operator-(Vector4 vec) const {
    Vector4 res(this->x - vec.x, this->y - vec.y, this->z - vec.z,
                this->w - vec.w);
    return res;
  };

  // --- MUDANÇA: Adicionado 'const' no final ---
  Vector4 operator+(Vector4 vec) const {
    Vector4 res(this->x + vec.x, this->y + vec.y, this->z + vec.z,
                this->w + vec.w);
    return res;
  }

  // --- MUDANÇA: Adicionado 'const' no final ---
  float lenght() const {
    float lenght =
        std::sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    return lenght;
  }

  // Esta função MODIFICA o objeto
  Vector4 normalize() {
    float lenght = this->lenght();
    if (lenght > 0) {
      this->x /= lenght;
      this->y /= lenght;
      this->z /= lenght;
    }
    this->w = 0; // Vetores devem ter w=0
    return *this;
  }

  // --- NOVO: Versão 'const' que retorna uma cópia ---
  Vector4 normalized() const {
    float lenght = this->lenght();
    if (lenght == 0) return Vector4(0,0,0,0);
    return Vector4(this->x / lenght, 
                   this->y / lenght, 
                   this->z / lenght, 
                   0.0f); // Vetores devem ter w=0
  }

  // --- MUDANÇA: Adicionado 'const' no final ---
  Vector4 operator*(float scalar) const {
    return Vector4(this->x * scalar, this->y * scalar, this->z * scalar,
                   this->w * scalar);
  }
};

#endif // !VECTOR4