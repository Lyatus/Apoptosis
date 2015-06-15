#include "Game.h"

#include "Conf.h"

using namespace L;

Map<String,float*> Game::_valueMap;
Time Game::_start, Game::_delta, Game::_since, Game::_now;

void Game::registerValue(const L::String& name, float* p) {
  *p = Conf::getFloat(name);
  _valueMap[name] = p;
}
Time Game::frame() {
  Time then(_now);
  _now = Time::now();
  _delta = _now-then;
  _since = _now-_start;
  return _delta;
}
void Game::start() {
  _start = _now = Time::now();
}
