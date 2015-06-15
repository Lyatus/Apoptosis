#include "Game.h"

#include "Conf.h"

using namespace L;

Map<String,float*> Game::_valueMap;
Time Game::_start;

void Game::registerValue(const L::String& name, float* p) {
  *p = Conf::getFloat(name);
  _valueMap[name] = p;
}
