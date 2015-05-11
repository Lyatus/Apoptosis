#ifndef DEF_Cancer_Sphere
#define DEF_Cancer_Sphere

#include <L/L.h>
#include "Shape.h"

class Sphere : public Shape {
  private:
    L::Point3f _center;
    float _radius;
  public:
    Sphere(const L::Point3f& center, float radius);
    virtual ~Sphere() {}

    L::Point3i start() const;
    float value(const L::Point3f&) const;
};

#endif



