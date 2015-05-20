#include "SphericalCamera.h"

#include "Conf.h"

using namespace L;

float SphericalCamera::_minAngle, SphericalCamera::_minFovy, SphericalCamera::_maxFovy;

void SphericalCamera::reset(const L::Point3f& point) {
  _interval.clear();
  addPoint(point-Point3f(8,8,8));
  addPoint(point+Point3f(8,8,8));
  position(point+Point3f(0,0,-128));
}
void SphericalCamera::fovy(float fovy) {
  _fovy = fovy;
  perspective(_fovy,Window::aspect(),.1f,512);
}
void SphericalCamera::addPoint(const L::Point3f& point) {
  _interval.add(point);
  _centerTarget = _interval.center();
  _radius = _interval.size().norm();
}
void SphericalCamera::update(World& world, float deltaTime) {
  Point3f oldPosition(position());
  if(!_interval.empty()) {
    _center += ((_centerTarget-_center)/1.1f)*deltaTime;
    lookat(_center);
    float distance(_centerTarget.dist(position()));
    move(Point3f(0,0,-((_radius-distance)/1.1f)*deltaTime));
  }
  if(Window::isPressed(Window::Event::LEFT))
    phiPosition(2*deltaTime);
  if(Window::isPressed(Window::Event::RIGHT))
    phiPosition(-2*deltaTime);
  if(forward().dot(Point3f(0,1,0))<.99f && Window::isPressed(Window::Event::UP))
    thetaPosition(2*deltaTime);
  if(forward().dot(Point3f(0,1,0))>-.99f && Window::isPressed(Window::Event::DOWN))
    thetaPosition(-2*deltaTime);
  if(world.spherecast(position(),4))
    position(oldPosition);
}
void SphericalCamera::event(World& world, const L::Window::Event& e) {
  static int x, y;
  if(e.type==Window::Event::MOUSEMOVE) {
    if(Window::isPressed(Window::Event::MBUTTON)) {
      Point3f oldPosition(position());
      phiPosition((e.x-x)*-.005f);
      thetaPosition((e.y-y)*-.005f);
      float forwardDotUp(forward().dot(Point3f(0,1,0))), forwardDotForward(forward().dot(Point3f(0,0,1)));
      std::cout << forwardDotForward << std::endl;
      if(world.spherecast(position(),4) || forwardDotUp>.99f || forwardDotUp<-.99f || forwardDotForward<_minAngle)
        position(oldPosition);
    }
    x = e.x;
    y = e.y;
  }
  if(e.type == Window::Event::MOUSEWHEEL)
    fovy(std::max(_minFovy,std::min(_maxFovy,_fovy+(float)-e.y/16)));
}
void SphericalCamera::configure() {
  _minAngle = Conf::getFloat("camera_min_angle");
  _minFovy = Conf::getFloat("camera_min_fovy");
  _maxFovy = Conf::getFloat("camera_max_fovy");
}
