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
    static float _minAngle, _minFovy, _maxFovy, _minFovx, _maxFovx, _audioViewportRadius;

  public:
    void reset(const L::Point3f&);
    void fovy(float);
    void addPoint(const L::Point3f&);
    bool update(World&, float deltaTime);
    void event(World&, const L::Window::Event&);

    inline const L::Point3f& center() const {return _center;}
    inline float radius() const {return _center.dist(position());}
    inline float fovx() const {return _fovx;}
    inline float listenerDistance() const {return _audioViewportRadius/tan(fovx()*.5f*L::DEGTORAD<float>());}
    inline L::Point3f listenerPosition() const {return position()+forward()*listenerDistance();}

    static void configure();
};

#endif



