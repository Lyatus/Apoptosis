#include "Bonus.h"

#include "Conf.h"
#include "Game.h"
#include "main.h"
#include "Resource.h"

using namespace L;

Array<Bonus> Bonus::_bonuses;
Color Bonus::_activeColor, Bonus::_inactiveColor, Bonus::_expiredColor;

Bonus::Bonus(const L::Dynamic::Var& v)
  : _position(Conf::getPointFrom(v["position"])),
    _duration(0), _end(0),
    _active(false), _timed(false) {
  if(v.as<Dynamic::Node>().has("icon"))
    _image = Resource::texture(v["icon"].as<String>());
  if(v.as<Dynamic::Node>().has("duration"))
    _duration = Time(v["duration"].as<float>()*1000000.f);
  if(v.as<Dynamic::Node>().has("tumors")) {
    if(_duration==0)
      _duration = -1;
    const Dynamic::Array& tumors(v["tumors"].as<Dynamic::Array>());
    for(int i(0); i<tumors.size(); i++)
      _tumors.push(Conf::getPointFrom(tumors[i]));
  }
  if(v.as<Dynamic::Node>().has("modifications")) {
    const Dynamic::Array& modifications(v["modifications"].as<Dynamic::Array>());
    for(int i(0); i<modifications.size(); i++) {
      _values.push(Game::value(modifications[i]["value"].as<String>()));
      if(modifications[i]["operation"].as<String>()=="mult")
        _operations.push(MULT);
      else _operations.push(ADD);
      _parameters.push(modifications[i]["parameter"].as<float>());
    }
  }
}
void Bonus::update(World& world) {
  Voxel voxel(world.voxel(_position.x(),_position.y(),_position.z()));
  bool tumored(world.spherecast(_position,8,[](Voxel v) {
    return v.type()==Voxel::TUMOR
           ||v.type()==Voxel::TUMOR_IDLE
           ||v.type()==Voxel::TUMOR_CHEMO;
  }));
  if(timed()) { // Timed bonus (or spawner)
    if(tumored && !_active && !activated()) {
      _end = Time::now()+_duration;
      activate();
    } else if(_active && Time::now()>_end)
      deactivate();
  } else { // Classic bonus
    if(tumored && !_active)
      activate();
    else if(!tumored && _active)
      deactivate();
  }
}
void Bonus::activate() {
  _active = true;
  for(int i(0); i<_values.size(); i++)
    switch(_operations[i]) {
      case ADD:
        *_values[i] += _parameters[i];
        break;
      case MULT:
        *_values[i] *= _parameters[i];
        break;
    }
  for(int i(0); i<_tumors.size(); i++)
    startGrowth(_tumors[i]);
}
void Bonus::deactivate() {
  _active = false;
  for(int i(0); i<_values.size(); i++)
    switch(_operations[i]) {
      case ADD:
        *_values[i] -= _parameters[i];
        break;
      case MULT:
        *_values[i] /= _parameters[i];
        break;
    }
}
void Bonus::draw(L::GL::Program& program, const SphericalCamera& cam) const {
  if(_image.null()) return; // There's nothing to draw (default cat pictures are not appreciated)
  if(_position.dist(cam.center())>cam.radius()*.75f) return;
  Point2f screenCenter;
  if(cam.worldToScreen(_position,screenCenter)) {
    Point3f worldTL(_position-cam.right()*4+cam.up()*4);
    Point2f screenOffset;
    cam.worldToScreen(worldTL,screenOffset);
    screenCenter = Window::normalizedToPixels(screenCenter);
    screenOffset = Window::normalizedToPixels(screenOffset);
    screenOffset -= screenCenter;
    if(_active)
      GL::color(_activeColor);
    else if(_end!=0)
      GL::color(_expiredColor);
    else
      GL::color(_inactiveColor);
    program.uniform("texture",*_image);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(screenCenter.x()-screenOffset.x(),screenCenter.y()-screenOffset.y());
    glTexCoord2f(0,1);
    glVertex2f(screenCenter.x()-screenOffset.x(),screenCenter.y()+screenOffset.y());
    glTexCoord2f(1,1);
    glVertex2f(screenCenter.x()+screenOffset.x(),screenCenter.y()+screenOffset.y());
    glTexCoord2f(1,0);
    glVertex2f(screenCenter.x()+screenOffset.x(),screenCenter.y()-screenOffset.y());
    glEnd();
  }
}
void Bonus::updateAll(World& world) {
  _bonuses.foreach([&world](Bonus& bonus) {
    bonus.update(world);
  });
}
void Bonus::drawAll(L::GL::Program& program, const SphericalCamera& cam) {
  _bonuses.foreach([&program,&cam](const Bonus& bonus) {
    bonus.draw(program,cam);
  });
}
float Bonus::distanceToActive(const Point3f& p) {
  float mag(std::numeric_limits<float>::max());
  _bonuses.foreach([&mag,&p](const Bonus& bonus) {
    if(bonus._active)
      mag = std::min(mag,bonus._position.distSquared(p));
  });
  return sqrt(mag);
}
float Bonus::distanceToInactive(const Point3f& p) {
  float mag(std::numeric_limits<float>::max());
  _bonuses.foreach([&mag,&p](const Bonus& bonus) {
    if(!bonus._active)
      mag = std::min(mag,bonus._position.distSquared(p));
  });
  return sqrt(mag);
}
void Bonus::configure() {
  const Dynamic::Array& bonuses(Conf::get()["bonuses"].as<Dynamic::Array>());
  for(int i(0); i<bonuses.size(); i++)
    _bonuses.push(bonuses[i]);
  _activeColor = Conf::getColor("bonus_active_color");
  _inactiveColor = Conf::getColor("bonus_inactive_color");
  _expiredColor = Conf::getColor("bonus_expired_color");
}
