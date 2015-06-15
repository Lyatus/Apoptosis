#ifndef DEF_Cancer_Game
#define DEF_Cancer_Game

#include <L/L.h>

class Game {
  private:
    static L::Map<L::String,float*> _valueMap;
    static L::Time _start;
  public:
    static void registerValue(const L::String&,float*);
    inline static float* value(const L::String& name) {return _valueMap[name];}
    inline static void start() {_start = L::Time::now();}
    inline static L::Time sinceStart() {return L::Time::now()-_start;}
};

#endif







