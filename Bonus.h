#ifndef DEF_Cancer_Bonus
#define DEF_Cancer_Bonus

#include <L/L.h>
#include "World.h"

class Bonus {
  private:
    static L::Map<L::String,float*> _valueMap;
    static L::Array<Bonus> _bonuses;
    static L::Map<L::String,L::Ref<L::GL::Texture> > _images;
    typedef enum{ADD,MULT} Operation;
    L::Point3f _position;
    L::Ref<L::GL::Texture> _image;
    L::Array<float*> _values;
    L::Array<Operation> _operations;
    L::Array<float> _parameters;
    float _radius;
    bool _active;

  public:
    Bonus() {}
    Bonus(const L::Dynamic::Var&);
    void update(World&);
    void activate();
    void deactivate();
    void draw(L::GL::Program&, const L::GL::Camera&) const;

    static void registerValue(const L::String&,float*);
    static void updateAll(World&);
    static void drawAll(L::GL::Program&,const L::GL::Camera&);
    static void configure();
};

#endif






