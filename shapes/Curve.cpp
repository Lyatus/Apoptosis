#include "Curve.h"

using namespace L;

Curve::Curve(const L::Point3f& a, const L::Point3f& b, const L::Point3f& c, const L::Point3f& d, float thickness, float step)
  : _a(a), _b(b), _c(c), _d(d) {
  for(float i(0); i<1.0; i+=step)
    _lines.push(at(i),at(std::min(1.f,i+step)),thickness);
}
L::Point3i Curve::start() const {
  return _a;
}
float Curve::value(const Point3f& p) const {
  float wtr(0);
  for(int i(0); i<_lines.size(); i++)
    wtr = std::max(wtr,_lines[i].value(p));
  return wtr;
}
Point3f Curve::at(float t) const {
  return _a*pow(1.f-t,3.f) + _b*3.f*t*pow(1.f-t,2.f) + _c*3.f*pow(t,2.f)*(1.f-t) + _d*pow(t,3.f);
}
