#include "Automaton.h"

using namespace L;
using namespace GL;

Array<Automaton*> Automaton::_automata;
L::Buffer<65536,Voxel> Automaton::_buffer;
L::Timer Automaton::_timer;

void Automaton::yield() {
  _timer.pause();
  Coroutine::yield();
  _timer.unpause();
}

Automaton::Automaton(World& world, Process process, float vps, const Time& end) : _world(world), _process(process), _vps(vps), _factor(0), _end(end), _shouldStop(false), _size(0) {}
void Automaton::include(const L::Point3i& p) {
  _zone.add(p-Point3i(1,1,1));
  _zone.add(p+Point3i(1,1,1));
}
void Automaton::update() {
  if(_zone.empty())
    _size = 0;
  else {
    Point3i ip, iw;
    int i(0), bufferMax;
    ip = iw = _min = _zone.min();
    _max = _zone.max()+Point3i(1,1,1);
    _size = std::max(1,_zone.size().product());
    bufferMax = _zone.size().y()*_zone.size().z();
    _zone.clear();
    bool processing(true);
    while(processing || !_buffer.empty()) {
      if(processing) {
        bool processable;
        _buffer.write(_process(*this,ip.x(),ip.y(),ip.z(),processable));
        if(processable)
          include(ip);
        if(!ip.increment(_min,_max))  // Increment returns false if we've gone back to the beginning
          processing = false;
      }
      if(_buffer.size()>bufferMax || !processing) {
        _world.updateVoxel(iw.x(),iw.y(),iw.z(),_buffer.read(),Voxel::set);
        _buffer.pop();
        iw.increment(_min,_max);
      }
      if(!(++i&0xFF))
        yield();
    }
  }
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
void Automaton::updateAll() {
  while(true) {
    _timer.setoff();
    fuse();
    for(int i(0); i<_automata.size(); i++)
      _automata[i]->update();
    for(int i(0); i<_automata.size(); i++)
      _automata[i]->_factor = std::max(.005f,_timer.since().fSeconds()*67.f/_automata[i]->_vps);
    clean();
    Coroutine::yield();
  }
}
void Automaton::fuse() {
  for(int i(0); i<_automata.size(); i++) // Check for automata that should fuse
    for(int j(0); j<_automata.size(); j++)
      if(i!=j && _automata[i]->_process==_automata[j]->_process
          && _automata[i]->shouldStop() == _automata[j]->shouldStop()
          && (_automata[i]->_zone && _automata[j]->_zone)) {
        Automaton *automaton(new Automaton(_automata[i]->_world,
                                           _automata[i]->_process,
                                           (_automata[i]->_vps+_automata[j]->_vps)/2,
                                           (_automata[i]->_end+_automata[j]->_end)/2));
        automaton->_zone = _automata[i]->_zone+_automata[j]->_zone;
        automaton->_size = std::max(1,automaton->_zone.size().product());
        remove(_automata[std::max(i,j)]);
        remove(_automata[std::min(i,j)]);
        add(automaton);
        return fuse();
      }
}
void Automaton::clean() {
  Time now(Time::now());
  for(int i(0); i<_automata.size(); i++) { // Check for automata that should be removed
    if(_automata[i]->_end<now) // Automaton should stop
      _automata[i]->_shouldStop = true;
    if(_automata[i]->_shouldStop && !_automata[i]->_size) // Automaton is ready to be removed
      remove(_automata[i--]);
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
bool Automaton::has(Process p) {
  for(int i(0); i<_automata.size(); i++)
    if(_automata[i]->_process==p)
      return true;
  return false;
}
bool Automaton::has(Process p, const Point3i& pt) {
  for(int i(0); i<_automata.size(); i++)
    if(_automata[i]->_process==p && _automata[i]->_zone.contains(pt))
      return true;
  return false;
}
bool Automaton::has(Process p, int count) {
  for(int i(0); i<_automata.size(); i++)
    if(_automata[i]->_process==p && --count<=0)
      return true;
  return false;
}
void Automaton::drawAll() {
  _automata.foreach([](Automaton*& a) {
    a->draw();
  });
}
/*
void Automaton::init() {
  for(int i(0); i<threadCount; i++)
    threads[i] = new std::thread(updateThread,i);
}
void Automaton::term() {
  for(int i(0); i<threadCount; i++)
    stopThread[i] = true;
  startSem.post(threadCount);
  for(int i(0); i<threadCount; i++) {
    threads[i]->join();
    delete threads[i];
  }
}
*/
