#ifndef DEF_Cancer_Automaton
#define DEF_Cancer_Automaton

#include <L/L.h>
#include <thread>
#include <mutex>
#include "World.h"

class Automaton {
  public:
    typedef Voxel(*Process)(Automaton&, int x, int y, int z, bool&);
  private:
    World& _world;
    Process _process;
    float _vps, _factor;
    L::Time _end;
    bool _shouldStop;

    L::Interval3i _zone;
    L::Point3i _min, _max;
    int _size;

    static L::Array<Automaton*> _automata;
    static L::Buffer<65536,Voxel> _buffer;
    static L::Timer _timer;

  public:
    Automaton(World&, Process, float vps, const L::Time& end = L::Time(0));
    void include(const L::Point3i&);
    void update();
    void draw() const;

    inline int size() const {return _size;}
    inline float factor() const {return _factor;}
    inline float vps() const {return _vps;}
    inline bool shouldStop() const { return _shouldStop;}
    inline Voxel voxel(int x, int y, int z) const {return _world.voxel(x,y,z);}

    static void updateAll();
    static void fuse();
    static void clean();
    static void add(Automaton*);
    static void remove(Automaton*);
    static bool has(Process);
    static bool has(Process,const L::Point3i&);
    static bool has(Process,int);
    static void drawAll();
};

#endif



