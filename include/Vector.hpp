#ifndef VECTOR4
#define VECTOR4

class Vector4 {
public:
  float x, y, z, w;
  Vector4(float x, float y, float z, float w);
  Vector4();

  float dot(Vector4 vec) {
    float res = this->x + vec.x;
    res += this->y + vec.y;
    res += this->z + vec.z;
    res += this->w + vec.w;
    return res;
  };

  Vector4 operator-(Vector4 vec) {
    Vector4 res(this->y - vec.x, this->y - vec.y, this->z - vec.z,
                this->w - vec.w);
    return res;
  };
  Vector4 operator+(Vector4 vec) {
    Vector4 res(this->y + vec.x, this->y + vec.y, this->z + vec.z,
                this->w + vec.w);
    return res;
  }
};

#endif // !VECTOR4
#define VECTOR4
