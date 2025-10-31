#include "../include/Color.hpp"

Color::Color(int r, int g, int b) {
  this->r = r;
  this->g = g;
  this->b = b;
}

Color::Color() {
  this->r = 200;
  this->g = 100;
  this->b = 50;
}
