#include "Automaton.h"

using namespace L;
using namespace GL;

Automaton::Automaton(World& world, Process process) : _world(world), _process(process), _size(0), _processing(false) {}
void Automaton::include(const L::Point3i& p) {
  _zone.add(p);
}
bool Automaton::update() {
  if(!_processing && _buffer.empty()) { // Ready for new processing
    if(_zone.empty()) {
      _size = 0;
      return false; // There is nothing to do
    }
    _ip = _iw = _min = _zone.min();
    _max = _zone.max()+Point3i(1,1,1);
    _size = (_max.x()-_min.x())*(_max.y()-_min.y())*(_max.z()-_min.z());
    _zone.clear();
    _processing = true;
  } else { // Still processing
    if(_processing) {
      _buffer.write(_process(_world,_ip.x(),_ip.y(),_ip.z()));
      if(!_ip.increment(_min,_max)) // Increment returns true if we've gone back to the beginning
        _processing = false;
    }
    if(_buffer.full() || !_processing) {
      Voxel v(_buffer.read());
      if(v.type()==Voxel::CANCER) {
        _zone.add(_iw-Point3i(1,1,1));
        _zone.add(_iw+Point3i(1,1,1));
      }
      _buffer.pop();
      _world.updateVoxel(_iw.x(),_iw.y(),_iw.z(),v,Voxel::set);
      _iw.increment(_min,_max);
    }
  }
  return true;
}

void Automaton::drawDebug() {
  if(_zone.empty()) return;
  glBegin(GL_QUADS);
  glColor3ub(255,255,0);
  // Left
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.min().z());
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.max().z());
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.max().z());
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.min().z());
  // Right
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.max().z());
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.max().z());
  // Bottom
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.max().z());
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.max().z());
  // Top
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.min().z());
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.max().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.max().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.min().z());
  // Front
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.min().z());
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.min().z());
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.min().z());
  // Back
  glVertex3i(_zone.min().x(),_zone.min().y(),_zone.max().z());
  glVertex3i(_zone.max().x(),_zone.min().y(),_zone.max().z());
  glVertex3i(_zone.max().x(),_zone.max().y(),_zone.max().z());
  glVertex3i(_zone.min().x(),_zone.max().y(),_zone.max().z());
  glEnd();
}

Voxel Automaton::rot(World& world, int x, int y, int z) {
  const Voxel& current(world.voxel(x,y,z));
  const Voxel& other(world.voxel(x+Rand::next(-1,2),y+Rand::next(-1,2),z+Rand::next(-1,2)));
  if((current.solid() || Rand::next(0,2)) && current.type()!=Voxel::CANCER) {
    if(other.solid() && other.type()==Voxel::CANCER)
      return Voxel(std::min(1.f,current.value()+Rand::next(0.f,.5f)),Voxel::CANCER);
  } else if(current.type()==Voxel::CANCER && !other.solid())
    return Voxel(std::max(0.f,current.value()-Rand::next(0.f,.01f)),Voxel::CANCER);
  return current; // No change
}
