#ifndef DEF_Cancer_Automaton
#define DEF_Cancer_Automaton

#include <L/L.h>
#include "World.h"

class Automaton {
  public:
    typedef Voxel(*Process)(Automaton&, int x, int y, int z, bool&);
  private:
    World& _world;
    L::Interval3i _zone;
    Process _process;
    L::Buffer<65536,Voxel> _buffer;
    float _vps, _factor;

    L::Point3i _min, _max, _ip, _iw;
    int _size;
    bool _processing;

    static L::Vector<Automaton*> _automata;

  public:
    Automaton(World&, Process, float vps);
    void include(const L::Point3i&);
    void update();
    void draw();

    inline int size() const {return _size;}
    inline float factor() const {return _factor;}
    inline float vps() const {return _vps;}
    inline Voxel voxel(int x, int y, int z) const {return _world.voxel(x,y,z);}

    static void update(const L::Time&, float deltaTime);
    static void add(Automaton*);
    static void remove(Automaton*);
    static void drawAll();
};

#endif



