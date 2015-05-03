#include "SphericalCamera.h"

using namespace L;

SphericalCamera::SphericalCamera(const L::Point3f& center) : GL::Camera(center+Point3f(0,0,1),center), _center(center), _centerTarget(center), _radius(1) {
}
void SphericalCamera::addPoint(const L::Point3f& point) {
  _interval.add(point);
  _centerTarget = _interval.center();
  _radius = _interval.size().norm();
}
void SphericalCamera::update(float deltaTime) {
  if(!_interval.empty()) {
    _center += ((_centerTarget-_center)/1.1)*deltaTime;
    lookat(_center);
    float distance(_centerTarget.dist(position()));
    move(Point3f(0,0,-((_radius-distance)/1.1)*deltaTime));
  }
  if(Window::isPressed(Window::Event::LEFT))
    phiPosition(2*deltaTime);
  if(Window::isPressed(Window::Event::RIGHT))
    phiPosition(-2*deltaTime);
  if(Window::isPressed(Window::Event::UP))
    thetaPosition(2*deltaTime);
  if(Window::isPressed(Window::Event::DOWN))
    thetaPosition(-2*deltaTime);
}
void SphericalCamera::event(const L::Window::Event& e) {
}
void SphericalCamera::configure() {
}
