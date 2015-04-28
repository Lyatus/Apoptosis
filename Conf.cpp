#include "Conf.h"

using namespace L;

Dynamic::Var Conf::_json;

void Conf::open(const String& path) {
  Interface<Dynamic::Var>::fromFile(_json,path);
}
String Conf::getString(const String& id) {
  return _json[id].get<String>();
}
Color Conf::getColor(const String& id) {
  return Color::from(getString(id));
}
float Conf::getFloat(const String& id) {
  return _json[id].get<float>();
}
int Conf::getInt(const String& id) {
  return getFloat(id);
}
bool Conf::getBool(const String& id) {
  return _json[id].get<bool>();
}
