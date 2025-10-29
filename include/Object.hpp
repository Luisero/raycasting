#ifndef OBJECT
#include "./Point.hpp"
#define OBJECT
class Object {
public:
  Point center;
  Object();
  virtual intersect(Ray ray);
};
#endif // !OBJECT
