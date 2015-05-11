#ifndef DEF_Cancer_Curve
#define DEF_Cancer_Curve

#include <L/L.h>
#include "Line.h"

class Curve : public Shape {
  private:
    L::Point3f _a, _b, _c, _d;
    L::Vector<Line> _lines;
  public:
    Curve(const L::Point3f& a, const L::Point3f& b, const L::Point3f& c, const L::Point3f& d, float thickness, float step);
    virtual ~Curve() {}

    L::Point3i start() const;
    float value(const L::Point3f&) const;
    L::Point3f at(float) const;
};

#endif



