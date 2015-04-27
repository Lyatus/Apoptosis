#include "Sphere.h"

using namespace L;

Sphere::Sphere(const Point3f& center, float radius)
  : Shape(true), _center(center), _radius(radius) {
}
L::Point3i Sphere::start() const {
  return _center;
}
float Sphere::value(const Point3f& p) const {
  return fromDistance(p.dist(_center)-_radius);
}
