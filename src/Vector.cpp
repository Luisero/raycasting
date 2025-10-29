#include "../include/Vector.hpp"

Vector4::Vector4(float x, float y, float z, float w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

Vector4::Vector4() {
  this->x = 0;
  this->y = 0;
  this->z = 0;
  this->w = 0;
}
