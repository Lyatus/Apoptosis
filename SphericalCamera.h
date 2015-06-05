#ifndef DEF_Cancer_SphericalCamera
#define DEF_Cancer_SphericalCamera

#include <L/L.h>
#include "World.h"

class SphericalCamera : public L::GL::Camera {
  private:
    L::Interval3f _interval;
    L::Point3f _center, _centerTarget;
    L::Point2f _speed, _mouse;
    float _radius, _fovy, _fovyTarget, _fovx;
    static float _minAngle, _minFovy, _maxFovy, _minFovx, _maxFovx;

  public:
    void reset(const L::Point3f&);
    void fovy(float);
    void addPoint(const L::Point3f&);
    bool update(World&, float deltaTime);
    void event(World&, const L::Window::Event&);

    inline const L::Point3f& center() const {return _center;}
    inline float radius() const {return _center.dist(position());}

    static void configure();
};

#endif



