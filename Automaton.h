#ifndef DEF_Cancer_Automaton
#define DEF_Cancer_Automaton

#include <L/L.h>
#include "World.h"

class Automaton {
  public:
    typedef Voxel(*Process)(World& world, int x, int y, int z, bool&);
  private:
    World& _world;
    L::Interval3i _zone;
    Process _process;
    L::Buffer<65536,Voxel> _buffer;

    L::Point3i _min, _max, _ip, _iw;
    int _size;
    bool _processing;
  public:
    Automaton(World&, Process);
    void include(const L::Point3i&);
    void update(int count = 1);
    inline int size() const {return _size;}
    void drawDebug();

    static Voxel rot(World& world, int x, int y, int z, bool&);
    template <int El, int Eu, int Fl, int Fu>
    static Voxel gol(World& world, int x, int y, int z) {
      const Voxel& current(world.voxel(x,y,z));
      int neighbors(0);
      for(int xi(x-1); xi<=x+1; xi++)
        for(int yi(y-1); yi<=y+1; yi++)
          for(int zi(z-1); zi<=z+1; zi++)
            if(world.voxel(xi,yi,zi).solid())
              neighbors++;
      if(current.solid()) { // Environment
        neighbors--;
        if(neighbors<El || neighbors >Eu) // Must die
          return Voxel();
        else // Survives
          return current;
      } else { // Fertility
        if(neighbors>=Fl && neighbors<=Fu)
          return Voxel(1.0);
      }
      return current; // No change
    }
};

#endif



