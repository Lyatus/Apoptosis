#ifndef DEF_Cancer_Voxel
#define DEF_Cancer_Voxel

#include <L/L.h>

class Voxel {
  public:
    typedef unsigned short T;
    static const T precisionBits = 10;
    static const T precisionMask = ~((~0)<<precisionBits);
    static const T precision = 1 << precisionBits;
    static const T typesBits = 4;
    static const T types = 1 << typesBits;
  private:
  T _value:
    precisionBits;
  T _type:
    typesBits;
  public:
    enum {
      NOTHING,
      CANCER, CANCER_IDLE,
      LUNG,
      VESSEL
    };
    Voxel(float value = 0, L::byte type = 0);
    bool operator==(const Voxel& other) const {return _type==other._type && _value==other._value;}
    bool operator!=(const Voxel& other) const {return _type!=other._type || _value!=other._value;}

    inline bool solid() const {return value()>.5f;}
    inline float value() const {return _value/(float)(precision-1);}
    inline L::byte type() const {return _type;}
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



