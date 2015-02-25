#ifndef DEF_Cancer_Automaton
#define DEF_Cancer_Automaton

#include <L/L.h>
#include "World.h"

class Automaton {
  public:
    static const int size = 32;
  private:
    L::Interval3i _zone;
    Voxel buffer[65536];
  public:
    Automaton(int x, int y, int z);

};

#endif



