#include "Line.h"

using namespace L;

Line::Line(const L::Point3f& a, const L::Point3f& b, float thickness)
  : Shape(true), _a(a), _b(b), _ab(_b-_a), _thickness(thickness), _abDotAb(_ab.dot(_ab)) {
}
L::Point3i Line::start() const {
  return _a;
}
float Line::value(const Point3f& p) const {
  float distance;
  Point3f ap(p - _a);
  float apDotAb(ap.dot(_ab));
  if(apDotAb <= 0) // p is before the segment
    distance = p.dist(_a);
  else if(_abDotAb <= apDotAb) // p is after the segment
    distance = p.dist(_b);
  else  // p can be projected onto the segment
    distance = p.dist(_a + _ab * (apDotAb / _abDotAb)); // Project p onto the segment and compute distance to p
  return fromDistance(distance-_thickness);
}
