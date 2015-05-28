#include "Event.h"

#include "Conf.h"
#include "Game.h"

using namespace L;

Array<Event> Event::_events;

Event::Event(const L::Dynamic::Var& v) {
  _value = Game::value(v["value"].as<String>());
  if(v["operation"].as<String>()=="mult")
    _operation = MULT;
  else _operation = ADD;
  _parameter = v["parameter"].as<float>();
  _time = Time(1000000.f*v["time"].as<float>());
}
bool Event::update(const Time& time) {
  if(_time<time) {
    switch(_operation) {
      case ADD:
        *_value += _parameter;
        break;
      case MULT:
        *_value *= _parameter;
        break;
    }
    return true;
  }
  return false;
}
void Event::updateAll(const Time& time) {
  for(int i(0); i<_events.size(); i++)
    if(_events[i].update(time)) {
      _events.erase(i--);
    }
}
void Event::configure() {
  const Dynamic::Array& events(Conf::get()["events"].as<Dynamic::Array>());
  for(int i(0); i<events.size(); i++)
    _events.push(events[i]);
}
