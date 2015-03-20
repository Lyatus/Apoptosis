#ifndef DEF_Cancer_Voxel
#define DEF_Cancer_Voxel

#include <L/L.h>

class Voxel {
  private:
    typedef unsigned short T;
    static const T precisionBits = 10;
    static const T precisionMask = ~((~0)<<precisionBits);
    static const T precision = 1 << precisionBits;
    T _v;
  public:
    enum {
      NOTHING,
      CANCER,
      LUNG,
      VESSEL
    };
    Voxel(float value = 0, byte type = 0);
    bool operator==(const Voxel& other) const {return _v==other._v;}
    bool operator!=(const Voxel& other) const {return _v!=other._v;}

    inline bool solid() const {return value()>.5f;}
    inline float value() const {return (_v & precisionMask)/(float)(precision-1);}
    inline byte type() const {return _v >> precisionBits;}
    L::Color color() const;

    // Updaters
    typedef Voxel(*Updater)(Voxel,Voxel);
    static Voxel set(Voxel,Voxel);
    static Voxel add(Voxel,Voxel);
    static Voxel sub(Voxel,Voxel);
    static Voxel min(Voxel,Voxel);
    static Voxel max(Voxel,Voxel);
};

#endif



