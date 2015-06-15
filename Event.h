#ifndef DEF_Cancer_Event
#define DEF_Cancer_Event

#include <L/L.h>
#include "SCA.h"
#include "World.h"

class Event {
  private:
    static L::Array<Event> _events;
    static World* _world;
    static SCA* _sca;
    typedef enum {ADD,MULT,VESSEL,TUMOR,WWISE} Operation;
    float* _value;
    Operation _operation;
    float _parameter;
    L::Point3f _position;
    L::String _name;
    L::Time _time;

  public:
    Event() {}
    Event(const L::Dynamic::Var&);
    bool update();
    static void updateAll();
    static void configure(World*, SCA*);
};

#endif






