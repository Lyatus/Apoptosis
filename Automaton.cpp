#include "Automaton.h"

using namespace L;
using namespace GL;

Array<Automaton*> Automaton::_automata;
std::thread* Automaton::threads[threadCount] = {0};
Semaphore Automaton::startSem(0), Automaton::endSem(0);
const Point3i Automaton::delta(threadCount,1,1);

Automaton::Automaton(World& world, Process process, float vps, const Time& end) : _world(world), _process(process), _vps(vps), _factor(0), _end(end), _shouldStop(false), _size(0) {}
void Automaton::include(const L::Point3i& p) {
  _zone.add(p);
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

void Automaton::updateThread(int id) {
  L::Buffer<65536,Voxel>* buffer(new L::Buffer<65536,Voxel>());
  Point3i min, max, ip, iw;
  while(true) {
    startSem.wait();
    for(int i(0); i<_automata.size(); i++) {
      Automaton& a(*_automata[i]);
      if(a._size) {
        bool processing(true);
        min = a._min;
        max = a._max;
        min.x() += id; // Interleave
        ip = iw = min;
        while(processing || !buffer->empty()) {
          if(processing) {
            bool processable;
            buffer->write(a._process(a,ip.x(),ip.y(),ip.z(),processable));
            if(processable) {
              a.include(ip-Point3i(1,1,1));
              a.include(ip+Point3i(1,1,1));
            }
            if(!ip.increment(min,max,delta)) // Increment returns false if we've gone back to the beginning
              processing = false;
          }
          if(buffer->full() || !processing) {
            a._world.updateVoxel(iw.x(),iw.y(),iw.z(),buffer->read(),Voxel::set);
            buffer->pop();
            iw.increment(min,max,delta);
          }
        }
      }
    }
    endSem.post();
  }
}
void Automaton::update(const Time& time, float deltaTime) {
  fuse();
  Timer atimer;
  int aturns(0);
  do {
    _automata.foreach([](Automaton*& a) {
      if(a->_zone.empty()) a->_size = 0;
      else {
        a->_min = a->_zone.min();
        a->_max = a->_zone.max()+Point3i(1,1,1);
        a->_size = std::max(1,a->_zone.size().product());
        a->_zone.clear();
      }
    });
    startSem.post(threadCount);
    endSem.wait(threadCount);
    aturns++;
  } while((atimer.since()*(aturns+1))/aturns<time);
  for(int i(0); i<_automata.size(); i++)
    _automata[i]->_factor = _automata[i]->_vps/(aturns/deltaTime);
  Time now(Time::now());
  for(int i(0); i<_automata.size(); i++) { // Check for automata that should be removed
    if(_automata[i]->_end<now) // Automaton should stop
      _automata[i]->_shouldStop = true;
    if(_automata[i]->_shouldStop && !_automata[i]->_size) // Automaton is ready to be removed
      remove(_automata[i--]);
  }
}
void Automaton::fuse() {
  for(int i(0); i<_automata.size(); i++) // Check for automata that should fuse
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
        return fuse();
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
  _automata.foreach([](Automaton*& a) {
    a->draw();
  });
}
void Automaton::init() {
  for(int i(0); i<threadCount; i++)
    threads[i] = new std::thread(updateThread,i);
}
