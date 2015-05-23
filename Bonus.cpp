#include "Bonus.h"

#include "Conf.h"

using namespace L;

Map<String,float*> Bonus::_values;
Array<Bonus> Bonus::_bonuses;
Map<String,Ref<GL::Texture> > Bonus::_images;

Bonus::Bonus(const L::Dynamic::Var& v)
  : _position(Conf::getPointFrom(v["position"])),
    _image(_images["test"]),
    _value(_values[v["value"].as<String>()]),
    _parameter(v["parameter"].as<float>()),
    _active(false) {
  if(v["operation"].as<String>()=="mult")
    _operation = MULT;
  else _operation = ADD;
}
void Bonus::update(World& world) {
  Voxel voxel(world.voxel(_position.x(),_position.y(),_position.z()));
  bool tumored(voxel.type()==Voxel::TUMOR_IDLE || voxel.type()==Voxel::TUMOR || voxel.type()==Voxel::VESSEL);
  if(tumored && !_active)
    activate();
  else if(!tumored && _active)
    deactivate();
}
void Bonus::activate() {
  _active = true;
  switch(_operation) {
    case ADD:
      *_value += _parameter;
      break;
    case MULT:
      *_value *= _parameter;
      break;
  }
}
void Bonus::deactivate() {
  _active = false;
  switch(_operation) {
    case ADD:
      *_value -= _parameter;
      break;
    case MULT:
      *_value /= _parameter;
      break;
  }
}
void Bonus::draw(L::GL::Program& program, const L::GL::Camera& cam) const {
  Point2f p(Window::normalizedToPixels(cam.worldToScreen(_position)));
  GL::color((_active)?Color::yellow:Color::cyan);
  program.uniform("texture",*_image);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex2f(p.x()-16,p.y()-16);
  glTexCoord2f(0,1);
  glVertex2f(p.x()-16,p.y()+16);
  glTexCoord2f(1,1);
  glVertex2f(p.x()+16,p.y()+16);
  glTexCoord2f(1,0);
  glVertex2f(p.x()+16,p.y()-16);
  glEnd();
}
void Bonus::registerValue(const L::String& name, float* p) {
  *p = Conf::getFloat(name);
  _values[name] = p;
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
void Bonus::configure() {
  _images["test"] = new GL::Texture(Image::Bitmap("Image/chat.png"));
  const Dynamic::Array& bonuses(Conf::get()["bonuses"].as<Dynamic::Array>());
  for(int i(0); i<bonuses.size(); i++)
    _bonuses.push(bonuses[i]);
}
