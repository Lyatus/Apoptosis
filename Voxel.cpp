#include "Voxel.h"

#include "Conf.h"

using namespace L;

L::Color Voxel::_colors[types];

Voxel::Voxel(float value, L::byte type)
  : _value(value*(precision-1)), _type(type) {}

void Voxel::configure() {
  _colors[ORGAN] = Conf::getColor("organ");
  _colors[VESSEL] = Conf::getColor("vessel");
  _colors[TUMOR] = _colors[TUMOR_IDLE] = _colors[TUMOR_START] = Conf::getColor("tumor");
  _colors[TUMOR_THIRSTY] = _colors[TUMOR_THIRSTY_IDLE] = Conf::getColor("tumor_thirsty");
  _colors[ORGAN_CHEMO] = _colors[TUMOR_CHEMO] = _colors[TUMOR_THIRSTY_CHEMO] = Conf::getColor("chemo");
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
