#ifndef DEF_Cancer_Triangle
#define DEF_Cancer_Triangle

#include <L/L.h>
#include "Line.h"

class Triangle : public Shape {
  private:
    Line _ab, _bc, _ac;
    L::Matrix44f _triangleMatrix;
    float _thickness;
  public:
    Triangle(const L::Point3f& a, const L::Point3f& b, const L::Point3f& c, float thickness);
    virtual ~Triangle() {}

    L::Point3i start() const;
    float value(const L::Point3f&) const;
};

#endif



