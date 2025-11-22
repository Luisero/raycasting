#ifndef VECTOR4
#define VECTOR4

#include <cmath>
class Vector4 {
public:
  float x, y, z, w;
  Vector4(float x, float y, float z, float w);
  Vector4();

  
  float dot(Vector4 vec) const {
    float res = this->x * vec.x;
    res += this->y * vec.y;
    res += this->z * vec.z;
    res += this->w * vec.w;
    return res;
  };

  
  Vector4 operator-(Vector4 vec) const {
    Vector4 res(this->x - vec.x, this->y - vec.y, this->z - vec.z,
                this->w - vec.w);
    return res;
  };

  
  Vector4 operator+(Vector4 vec) const {
    Vector4 res(this->x + vec.x, this->y + vec.y, this->z + vec.z,
                this->w + vec.w);
    return res;
  }

  
  float lenght() const {
    float lenght =
        std::sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    return lenght;
  }

  
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

  
  Vector4 normalized() const {
    float lenght = this->lenght();
    if (lenght == 0) return Vector4(0,0,0,0);
    return Vector4(this->x / lenght, 
                   this->y / lenght, 
                   this->z / lenght, 
                   0.0f); // Vetores devem ter w=0
  }

  
  Vector4 operator*(float scalar) const {
    return Vector4(this->x * scalar, this->y * scalar, this->z * scalar,
                   this->w * scalar);
  }

  Vector4 cross(const Vector4& v) const {
    return Vector4(
      this->y * v.z - this->z * v.y, // Novo X
      this->z * v.x - this->x * v.z, // Novo Y
      this->x * v.y - this->y * v.x, // Novo Z
      0.0f                           // w = 0 para vetores
    );
  }
};

#endif // !VECTOR4