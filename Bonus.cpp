#include "Bonus.h"

#include "Conf.h"
#include "Game.h"

using namespace L;

Array<Bonus> Bonus::_bonuses;
Map<String,Ref<GL::Texture> > Bonus::_images;

Bonus::Bonus(const L::Dynamic::Var& v)
  : _position(Conf::getPointFrom(v["position"])),
    _end(0),
    _active(false), _timed(false) {
  if(v.as<Dynamic::Node>().has("icon")) {
    if(!_images.has(v["icon"].as<String>()))
      _image = _images[v["icon"].as<String>()] = new GL::Texture(Image::Bitmap(v["icon"].as<String>()));
    else _image = _images[v["icon"].as<String>()];
  } else _image = _images["default"];
  if(v.as<Dynamic::Node>().has("duration")) {
    _timed = true;
    _duration = Time(v["duration"].as<float>()*1000000.f);
  }
  const Dynamic::Array& modifications(v["modifications"].as<Dynamic::Array>());
  for(int i(0); i<modifications.size(); i++) {
    _values.push(Game::value(modifications[i]["value"].as<String>()));
    if(modifications[i]["operation"].as<String>()=="mult")
      _operations.push(MULT);
    else _operations.push(ADD);
    _parameters.push(modifications[i]["parameter"].as<float>());
  }
}
void Bonus::update(World& world) {
  Voxel voxel(world.voxel(_position.x(),_position.y(),_position.z()));
  bool tumored(world.spherecast(_position,8,[](Voxel voxel) {
    return voxel.type()==Voxel::TUMOR_IDLE || voxel.type()==Voxel::TUMOR;
  }));
  if(tumored && !_active)
    activate();
  else if(!tumored && _active)
    deactivate();
  if(_active && _timed && Time::now()>_end)
    deactivate();
}
void Bonus::activate() {
  _active = true;
  if(_timed && _end==0)
    _end = Time::now()+_duration;
  for(int i(0); i<_values.size(); i++)
    switch(_operations[i]) {
      case ADD:
        *_values[i] += _parameters[i];
        break;
      case MULT:
        *_values[i] *= _parameters[i];
        break;
    }
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
void Bonus::draw(L::GL::Program& program, const L::GL::Camera& cam) const {
  Point2f screenCenter;
  if(cam.worldToScreen(_position,screenCenter)) {
    Point3f worldTL(_position-cam.right()*4+cam.up()*4);
    Point2f screenOffset;
    cam.worldToScreen(worldTL,screenOffset);
    screenCenter = Window::normalizedToPixels(screenCenter);
    screenOffset = Window::normalizedToPixels(screenOffset);
    screenOffset -= screenCenter;
    if(_active)
      GL::color(Color(255,255,255,128));
    else if(_end!=0)
      GL::color(Color(255,0,0,128));
    else
      GL::color(Color::white);
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
void Bonus::drawAll(L::GL::Program& program, const L::GL::Camera& cam) {
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
  _images["default"] = new GL::Texture(Image::Bitmap("Image/chat.png"));
  const Dynamic::Array& bonuses(Conf::get()["bonuses"].as<Dynamic::Array>());
  for(int i(0); i<bonuses.size(); i++)
    _bonuses.push(bonuses[i]);
}
