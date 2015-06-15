#ifndef DEF_Cancer_Game
#define DEF_Cancer_Game

#include <L/L.h>

class Game {
  private:
    static L::Map<L::String,float*> _valueMap;
    static L::Time _start, _delta, _since, _now;
  public:
    static void registerValue(const L::String&,float*);
    inline static float* value(const L::String& name) {return _valueMap[name];}
    static L::Time frame();
    static void start();
    inline static const L::Time& sinceStart() {return _since;}
    inline static const L::Time& now() {return _now;}
};

#endif
