#include "Voxel.h"

using namespace L;
using namespace GL;

Voxel::Voxel(float value, L::byte type)
  : _v(type*precision+(T)(value*(precision-1))) {}
Color Voxel::color() const {
  switch(type()) {
    case CANCER:
      return Color(100,50,50);
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
  return Voxel(std::max(0.f,std::min(a.value(),b.value())),b.type());
}
Voxel Voxel::max(Voxel a,Voxel b) {
  return Voxel(std::max(0.f,std::max(a.value(),b.value())),b.type());
}
