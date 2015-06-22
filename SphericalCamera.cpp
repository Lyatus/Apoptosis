#include "SphericalCamera.h"

#include "Conf.h"
#include "Game.h"

using namespace L;

float SphericalCamera::_minAngle, SphericalCamera::_minFovy, SphericalCamera::_maxFovy, SphericalCamera::_minFovx, SphericalCamera::_maxFovx, SphericalCamera::_audioViewportRadius;

void SphericalCamera::reset(const L::Point3f& point) {
  _interval.clear();
  _centerTarget = _center = point;
  addPoint(point-Point3f(16,16,16));
  addPoint(point+Point3f(16,16,16));
  position(point+Point3f(0,0,-64));
  _fovyTarget = _maxFovy;
  fovy(_maxFovy);
}
void SphericalCamera::fovy(float fovy) {
  _fovy = fovy;
  _fovx = _fovy*Window::aspect();
  perspective(_fovy,Window::aspect(),.1f,512);
}
void SphericalCamera::addPoint(const L::Point3f& point) {
  _interval.add(point);
  _centerTarget = _interval.center();
  _radius = _interval.size().norm();
}
bool SphericalCamera::update(World& world, float deltaTime) {
  Point3f oldPosition(position());
  if(_fovyTarget!=_fovy)
    fovy(_fovy+(_fovyTarget-_fovy)*10.f*deltaTime);
  _mouse += (Window::normalizedMousePosition()-_mouse)*5.f*deltaTime;
  if(!_interval.empty()) {
    _center += ((_centerTarget-_center)/1.1f)*deltaTime;
    lookat(_center);
    float distance(_centerTarget.dist(position()));
    move(Point3f(0,0,-((_radius-distance)/1.1f)*deltaTime));
    phiLook(-_mouse.x()*((_maxFovx-_fovx)/2)*DEGTORAD<float>());
    thetaLook(_mouse.y()*((_maxFovy-_fovy)/2)*DEGTORAD<float>());
  }
  phiPosition(_speed.x()*-deltaTime);
  thetaPosition(_speed.y()*-deltaTime);
  _speed *= 1-deltaTime*10;
  float forwardDotUp(forward().dot(Point3f(0,1,0))), forwardDotForward(forward().dot(Point3f(0,0,1)));
  if(world.spherecast(position(),16) || forwardDotUp>.99f || forwardDotUp<-.99f || forwardDotForward<_minAngle)
    position(oldPosition);
  return _speed.norm()>.5f;
}
void SphericalCamera::event(World& world, const L::Window::Event& e) {
  static int x, y;
  if(e.type==Window::Event::MOUSEMOVE) {
    if(Window::isPressed(Window::Event::RBUTTON)) {
      _speed.x() += (e.x-x)*.035f;
      _speed.y() += (e.y-y)*.035f;
    }
    x = e.x;
    y = e.y;
  }
  if(e.type == Window::Event::MOUSEWHEEL)
    _fovyTarget = std::max(_minFovy,std::min(_maxFovy,_fovyTarget+(float)-e.y/16));
}
void SphericalCamera::configure() {
  Game::registerValue("camera_min_angle",&_minAngle);
  Game::registerValue("camera_min_fovy",&_minFovy);
  Game::registerValue("camera_max_fovy",&_maxFovy);
  _minFovx = _minFovy*Window::aspect();
  _maxFovx = _maxFovy*Window::aspect();
  _audioViewportRadius = Conf::getFloat("audio_viewport_radius");
}
