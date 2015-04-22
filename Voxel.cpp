#include "Voxel.h"

using namespace L;
using namespace GL;

Voxel::Voxel(float value, L::byte type)
  : _value(value*(precision-1)), _type(type) {}
Color Voxel::color() const {
  switch(type()) {
    case CANCER:
      return Color(128,64,64);
    case LUNG:
      return Color(255,128,128);
    case VESSEL:
      return Color(128,128,255);
    default :
      return Color::random();
  }
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
Voxel Voxel::min(Voxel a,Voxel b) {
  return (a.value()<b.value())?a:b;
}
Voxel Voxel::max(Voxel a,Voxel b) {
  return (a.value()>b.value())?a:b;
}
