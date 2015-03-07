#ifndef DEF_Cancer_Voxel
#define DEF_Cancer_Voxel

#include <L/L.h>

class Voxel {
  private:
    static const int precision = 64;
    byte _v;
  public:
    static const byte NOTHING = 0;
    static const byte CANCER = 1;
    static const byte LUNG = 2;
    Voxel(float value = 0, byte type = 0);
    bool operator==(const Voxel& other) const {return _v==other._v;}
    bool operator!=(const Voxel& other) const {return _v!=other._v;}

    bool solid() const;
    float value() const;
    byte type() const;
    L::Color color() const;

    // Updaters
    typedef Voxel(*Updater)(Voxel,Voxel);
    static Voxel set(Voxel,Voxel);
    static Voxel add(Voxel,Voxel);
    static Voxel sub(Voxel,Voxel);
};

#endif



