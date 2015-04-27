#ifndef DEF_Cancer_Shape
#define DEF_Cancer_Shape

#include <L/L.h>

class Shape {
  protected:
    bool _convex;
  public:
    Shape(bool convex = false) : _convex(convex) {}
    virtual L::Point3i start() const = 0;
    virtual float value(const L::Point3f&) const = 0;
    inline bool convex() const {return _convex;}
    inline float fromDistance(float distance) const {
      return std::min(1.f,std::max(0.f,.5f-distance));
    }
};

#endif



