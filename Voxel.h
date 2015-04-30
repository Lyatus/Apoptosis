#ifndef DEF_Cancer_Voxel
#define DEF_Cancer_Voxel

#include <L/L.h>

class Voxel {
  public:
    typedef unsigned short T;
    static const T precisionBits = 10;
    static const T precision = 1 << precisionBits;
    static const T precisionMax = precision-1;
    static const T precisionHalf = precision>>1;
    static const T typesBits = 4;
    static const T types = 1 << typesBits;
  private:
  T _value:
    precisionBits;
  T _type:
    typesBits;
    static L::Color _colors[types];
  public:
    enum {
      NOTHING, ORGAN, VESSEL,
      TUMOR, TUMOR_IDLE, TUMOR_THIRSTY, TUMOR_THIRSTY_IDLE
    };
    Voxel(float value = 0, L::byte type = 0);
    bool operator==(const Voxel& other) const {return _type==other._type && _value==other._value;}
    bool operator!=(const Voxel& other) const {return _type!=other._type || _value!=other._value;}

    inline bool solid() const {return _value>precisionHalf;}
    inline bool empty() const {return !_value;}
    inline bool full() const {return _value==precisionMax;}
    inline float value() const {return _value/(float)(precision-1);}
    inline L::byte type() const {return _type;}
    inline void value(float value) {_value = value*(precision-1);}
    inline void type(L::byte type) {_type = type;}
    L::Color color() const;

    static void configure();

    // Updaters
    typedef Voxel(*Updater)(Voxel,Voxel);
    static Voxel set(Voxel,Voxel);
    static Voxel add(Voxel,Voxel);
    static Voxel sub(Voxel,Voxel);
    static Voxel min(Voxel,Voxel);
    static Voxel max(Voxel,Voxel);
};

#endif



