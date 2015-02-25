#include "Voxel.h"

using namespace L;
using namespace GL;

Voxel::Voxel(float value, byte type)
  : _v(type*precision+value*(precision-1)) {}
bool Voxel::isSolid() const {
  return value()>.5f;
}
float Voxel::value() const {
  return (_v % precision)/(float)precision;
}
byte Voxel::type() const {
  return _v / precision;
}
Color Voxel::color() const{
  return Color::random();
}

Voxel Voxel::set(Voxel a,Voxel b) {
  return b;
}
Voxel Voxel::add(Voxel a,Voxel b) {
  return Voxel(std::min(1.f,a.value()+b.value()),b.type());
}
Voxel Voxel::sub(Voxel a,Voxel b) {
  return Voxel(std::max(0.f,a.value()-b.value()),b.type());
}
