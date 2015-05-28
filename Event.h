#ifndef DEF_Cancer_Event
#define DEF_Cancer_Event

#include <L/L.h>

class Event {
  private:
    static L::Array<Event> _events;
    typedef enum {ADD,MULT} Operation;
    float* _value;
    Operation _operation;
    float _parameter;
    L::Time _time;

  public:
    Event() {}
    Event(const L::Dynamic::Var&);
    bool update(const L::Time&);
    static void updateAll(const L::Time&);
    static void configure();
};

#endif






