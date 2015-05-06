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
Point3f Conf::getPoint(const String& id) {
  List<String> components(getString(id).explode(','));
  return Point3f(FromString<float>(components[0]),FromString<float>(components[1]),FromString<float>(components[2]));
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
