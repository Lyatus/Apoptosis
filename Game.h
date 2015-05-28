#ifndef DEF_Cancer_Game
#define DEF_Cancer_Game

#include <L/L.h>

class Game {
  private:
    static L::Map<L::String,float*> _valueMap;
  public:
    static void registerValue(const L::String&,float*);
    inline static float* value(const L::String& name) {return _valueMap[name];}
};

#endif







