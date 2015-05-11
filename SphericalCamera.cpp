#include "SphericalCamera.h"

using namespace L;

void SphericalCamera::reset(const L::Point3f& point) {
  _interval.clear();
  addPoint(point-Point3f(8,8,8));
  addPoint(point+Point3f(8,8,8));
  position(point+Point3f(0,0,-32));
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
      float forwardDotUp(forward().dot(Point3f(0,1,0)));
      if(world.spherecast(position(),4) || forwardDotUp>.99f || forwardDotUp<-.99f)
        position(oldPosition);
    }
    x = e.x;
    y = e.y;
  }
}
void SphericalCamera::configure() {
}
