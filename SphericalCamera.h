#ifndef DEF_Cancer_SphericalCamera
#define DEF_Cancer_SphericalCamera

#include <L/L.h>

class SphericalCamera : public L::GL::Camera {
  private:
    L::Interval3f _interval;
    L::Point3f _center, _centerTarget;
    float _radius;

  public:
    SphericalCamera(const L::Point3f&);
    void addPoint(const L::Point3f&);
    void update(float deltaTime);
    void event(const L::Window::Event&);
    static void configure();
};

#endif



