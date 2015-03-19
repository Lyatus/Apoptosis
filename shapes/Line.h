#ifndef DEF_Cancer_Line
#define DEF_Cancer_Line

#include <L/L.h>
#include "Shape.h"

class Line : public Shape {
  private:
    L::Point3f _a, _b, _ab;
    float _thickness, _abDotAb;
  public:
    Line(const L::Point3f& a, const L::Point3f& b, float thickness);
    L::Point3i start() const;
    float value(const L::Point3f&) const;
};

#endif



