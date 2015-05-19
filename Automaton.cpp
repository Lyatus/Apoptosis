#include "Automaton.h"

using namespace L;
using namespace GL;

Array<Automaton*> Automaton::_automata;

Automaton::Automaton(World& world, Process process, float vps, const Time& end) : _world(world), _process(process), _vps(vps), _factor(0), _end(end), _shouldStop(false), _size(0), _processing(false) {}
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

void Automaton::draw() const {
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
  Timer atimer;
  int aturns(0);
  do {
    _automata.foreach([](Automaton*& a) {a->update();});
    aturns++;
  } while((atimer.since()*(aturns+1))/aturns<time);
  for(int i(0); i<_automata.size(); i++)
    _automata[i]->_factor = _automata[i]->_vps/(aturns/deltaTime);
  Time now(Time::now());
  for(int i(0); i<_automata.size(); i++) { // Check for automata that should be removed
    if(_automata[i]->_end<now) // Automaton should stop
      _automata[i]->_shouldStop = true;
    if(_automata[i]->_shouldStop && !_automata[i]->_size) // Automaton is ready to be removed
      remove(_automata[i]);
  }
  for(int i(0); i<_automata.size(); i++)
    for(int j(0); j<_automata.size(); j++)
      if(i!=j && _automata[i]->_process==_automata[j]->_process
          && (_automata[i]->_zone && _automata[j]->_zone)) {
        Automaton *automaton(new Automaton(_automata[i]->_world,
                                           _automata[i]->_process,
                                           (_automata[i]->_vps+_automata[j]->_vps)/2,
                                           (_automata[i]->_end+_automata[j]->_end)/2));
        automaton->_zone = _automata[i]->_zone+_automata[j]->_zone;
        add(automaton);
        remove(_automata[std::max(i,j)]);
        remove(_automata[std::min(i,j)]);
        i = j = 0;
      }
}
void Automaton::add(Automaton* a) {
  _automata.push(a);
}
void Automaton::remove(Automaton* a) {
  for(int i(0); i<_automata.size(); i++)
    if(_automata[i]==a) {
      _automata.erase(i);
      delete a;
      return;
    }
}
void Automaton::drawAll() {
  /*_automata.foreach([](const Automaton*& a) {
    a->draw();
  });*/
}
