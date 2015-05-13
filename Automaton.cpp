#include "Automaton.h"

using namespace L;
using namespace GL;

Vector<Automaton*> Automaton::_automata;

Automaton::Automaton(World& world, Process process, float vps) : _world(world), _process(process), _vps(vps), _factor(0), _size(0), _processing(false) {}
void Automaton::include(const L::Point3i& p) {
  _zone.add(p);
}
void Automaton::update() {
  do {
    if(!_processing && _buffer.empty()) { // Ready for new processing
      if(_zone.empty())
        _size = 0;
      else {
        _ip = _iw = _min = _zone.min();
        _max = _zone.max()+Point3i(1,1,1);
        _size = _zone.size().product();
        _zone.clear();
        _processing = true;
      }
    } else { // Still processing
      if(_processing) {
        bool processable;
        _buffer.write(_process(*this,_ip.x(),_ip.y(),_ip.z(),processable));
        if(processable) {
          _zone.add(_ip-Point3i(1,1,1));
          _zone.add(_ip+Point3i(1,1,1));
        }
        if(!_ip.increment(_min,_max)) { // Increment returns false if we've gone back to the beginning
          _processing = false;
        }
      }
      if(_buffer.full() || !_processing) {
        Voxel v(_buffer.read());
        _buffer.pop();
        _world.updateVoxel(_iw.x(),_iw.y(),_iw.z(),v,Voxel::set);
        _iw.increment(_min,_max);
      }
    }
  } while(_processing || !_buffer.empty());
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

void Automaton::update(const Time& time, float deltaTime) {
  int aturns(0);
  Timer atimer;
  do {
    for(auto&& a : _automata)
      a->update();
    aturns++;
  } while((atimer.since()*(aturns+1))/aturns<time);
  for(auto&& a : _automata)
    a->_factor = a->_vps/(aturns/deltaTime);
}
void Automaton::add(Automaton* a) {
  _automata.push_back(a);
}
void Automaton::remove(Automaton* a) {
  for(int i(0); i<_automata.size(); i++)
    if(_automata[i]==a) {
      _automata.erase(_automata.begin()+i);
      return;
    }
}
