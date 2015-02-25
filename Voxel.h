#ifndef DEF_Cancer_Voxel
#define DEF_Cancer_Voxel

#include <L/L.h>

class Voxel {
  private:
    static const int precision = 32;
    byte _v;
  public:
    Voxel(float value = 0, byte type = 0);
    bool isSolid() const;
    float value() const;
    byte type() const;
    L::Color color() const;

    // Updaters
    typedef Voxel (*Updater)(Voxel,Voxel);
    static Voxel set(Voxel,Voxel);
    static Voxel add(Voxel,Voxel);
    static Voxel sub(Voxel,Voxel);
};

#endif



