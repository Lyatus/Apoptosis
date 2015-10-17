#ifndef DEF_Cancer_Bonus
#define DEF_Cancer_Bonus

#include <L/L.h>
#include "World.h"
#include "SphericalCamera.h"

class Bonus {
  private:
    static L::Array<Bonus> _bonuses;
    static L::Color _activeColor, _inactiveColor, _expiredColor;
    static float _radius;
    typedef enum {ADD,MULT} Operation;
    L::Point3f _position;
    L::Ref<L::GL::Texture> _image;
    L::Array<float*> _values;
    L::Array<Operation> _operations;
    L::Array<float> _parameters;
    L::Array<L::Point3f> _tumors;
    L::Time _duration, _end, _appearance;
    bool _active, _timed;

  public:
    Bonus() {}
    Bonus(const L::Var&);
    void update(World&);
    void activate();
    void deactivate();
    void draw(L::GL::Program&, const SphericalCamera&) const;

    inline bool timed() const {return _duration!=0;}
    inline bool spawner() const {return !_tumors.empty();}
    inline bool activated() const {return _end!=0;}

    static void updateAll(World&);
    static void drawAll(L::GL::Program&,const SphericalCamera&);
    static float distanceToActive(const L::Point3f&);
    static float distanceToInactive(const L::Point3f&);
    static void configure();
};

#endif






