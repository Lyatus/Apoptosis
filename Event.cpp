#include "Event.h"

#include "main.h"
#include "Conf.h"
#include "Game.h"
#include "Automaton.h"

using namespace L;

Array<Event> Event::_events;
World* Event::_world;
SCA* Event::_sca;

Event::Event(const L::Dynamic::Var& v) {
  _time = Time(1000000.f*v["time"].as<float>());
  if(v["operation"].as<String>()=="mult")
    _operation = MULT;
  else if(v["operation"].as<String>()=="add")
    _operation = ADD;
  else if(v["operation"].as<String>()=="vessel")
    _operation = VESSEL;
  else if(v["operation"].as<String>()=="tumor")
    _operation = TUMOR;
  else if(v["operation"].as<String>()=="wwise")
    _operation = WWISE;
  else throw Exception("Unkwown event operation");
  if(_operation==ADD || _operation==MULT) {
    _parameter = v["parameter"].as<float>();
    _value = Game::value(v["value"].as<String>());
  } else _position = Conf::getPointFrom(v["position"]);
}
bool Event::update() {
  if(_time<Game::sinceStart()) {
    switch(_operation) {
      case ADD:
        *_value += _parameter;
        break;
      case MULT:
        *_value *= _parameter;
        break;
      case VESSEL:
        _sca->addTarget(_position);
        break;
      case TUMOR:
        startGrowth(_position);
        break;
      case WWISE:
        Wwise::postEvent(_name);
        break;
    }
    return true;
  }
  return false;
}
void Event::updateAll() {
  for(int i(0); i<_events.size(); i++)
    if(_events[i].update())
      _events.erase(i--);
}
void Event::configure(World* world, SCA* sca) {
  _world = world;
  _sca = sca;
  const Dynamic::Array& events(Conf::get()["events"].as<Dynamic::Array>());
  for(int i(0); i<events.size(); i++)
    _events.push(events[i]);
}
