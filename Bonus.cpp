#include "Bonus.h"

#include "Conf.h"

using namespace L;

Map<String,float*> Bonus::_values;
Array<Bonus> Bonus::_bonuses;

Bonus::Bonus(const L::Dynamic::Var& v)
  : _position(Conf::getPointFrom(v["position"])),
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
void Bonus::draw() const {
  GL::color((_active)?Color::yellow:Color::cyan);
  glPointSize(32);
  glBegin(GL_POINTS);
  glVertex3f(_position.x(),_position.y(),_position.z());
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
void Bonus::drawAll() {
  _bonuses.foreach([](const Bonus& bonus) {
    bonus.draw();
  });
}
void Bonus::configure() {
  const Dynamic::Array& bonuses(Conf::get()["bonuses"].as<Dynamic::Array>());
  for(int i(0); i<bonuses.size(); i++)
    _bonuses.push(bonuses[i]);
}
