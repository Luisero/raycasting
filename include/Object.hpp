#ifndef OBJECT
#define OBJECT
#include "./Point.hpp"
#include "./Ray.hpp"
class Object {
public:
  Point center;
  Object();
  virtual float intersect(Ray ray) = 0;
};
#endif
