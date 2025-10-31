#ifndef VECTOR4
#define VECTOR4

#include <cmath>
class Vector4 {
public:
  float x, y, z, w;
  Vector4(float x, float y, float z, float w);
  Vector4();

  float dot(Vector4 vec) {
    float res = this->x * vec.x;
    res += this->y * vec.y;
    res += this->z * vec.z;
    res += this->w * vec.w;
    return res;
  };

  Vector4 operator-(Vector4 vec) {
    Vector4 res(this->x - vec.x, this->y - vec.y, this->z - vec.z,
                this->w - vec.w);
    return res;
  };
  Vector4 operator+(Vector4 vec) {
    Vector4 res(this->x + vec.x, this->y + vec.y, this->z + vec.z,
                this->w + vec.w);
    return res;
  }
  float lenght() {
    float lenght =
        std::sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    return lenght;
  }
  Vector4 normalize() {
    float lenght = this->lenght();
    this->x /= lenght;
    this->y /= lenght;
    this->z /= lenght;
    return Vector4(this->x, this->y, this->z, 0);
  }
  Vector4 operator*(float scalar) {
    return Vector4(this->x * scalar, this->y * scalar, this->z * scalar,
                   this->w * scalar);
  }
};

#endif // !VECTOR4
