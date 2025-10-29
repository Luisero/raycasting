#include "../include/Object.hpp"

Object::Object() { this->center = Point(0.0f, 0.0f, 0.0f, 1.0f); }
Object::~Object() {}

Vector4 Object::getNormal(Point collide) { return Vector4(0.f, 0.f, 0.f, 0.f); }
