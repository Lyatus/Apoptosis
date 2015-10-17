#include "Conf.h"

using namespace L;

Var Conf::_json;

void Conf::open(const String& path) {
  Interface<Var>::fromFile(_json,path);
}
void Conf::load(const char* str) {
  Interface<Var>::in("json").from(_json,str);
}
const Var& Conf::get() {
  return _json;
}
String Conf::getString(const String& id) {
  return _json[id].get<String>();
}
String Conf::getStringFrom(const Var& v) {
  return v.as<String>();
}
Color Conf::getColor(const String& id) {
  return Color::from(getString(id));
}
Point3f Conf::getPoint(const String& id) {
  return getPointFrom(_json[id]);
}
Point3f Conf::getPointFrom(const Var& v) {
  Array<String> components(getStringFrom(v).explode(','));
  return Point3f(FromString<float>(components[0]),FromString<float>(components[1]),FromString<float>(components[2]));
}

float Conf::getFloat(const String& id) {
  return getFloatFrom(_json[id]);
}
float Conf::getFloatFrom(const Var& v) {
  return v.as<float>();
}
int Conf::getInt(const String& id) {
  return getFloat(id);
}
bool Conf::getBool(const String& id) {
  return _json[id].get<bool>();
}
