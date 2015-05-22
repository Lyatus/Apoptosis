#ifndef DEF_Cancer_Bonus
#define DEF_Cancer_Bonus

#include <L/L.h>
#include "World.h"

class Bonus {
  private:
    static L::Map<L::String,float*> _values;
    static L::Array<Bonus> _bonuses;
    static L::Map<L::String,L::Ref<L::GL::Texture> > _images;

    L::Point3f _position;
    float _radius;
    float* _value;
    float _parameter;
    bool _active;
    enum {
      ADD, MULT
    } _operation;

  public:
    Bonus() {}
    Bonus(const L::Dynamic::Var&);
    void update(World&);
    void activate();
    void deactivate();
    void draw(const L::GL::Camera&) const;

    static void registerValue(const L::String&,float*);
    static void updateAll(World&);
    static void drawAll(const L::GL::Camera&);
    static void configure();
};

#endif



