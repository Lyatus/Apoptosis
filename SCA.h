#ifndef DEF_Cancer_SCA
#define DEF_Cancer_SCA

#include <L/L.h>
#include "World.h"
#include "shapes/Line.h"

class SCA {
  public:
    class Branch {
      private:
        Branch* _parent;
        L::Point3f _position, _direction, _originalDirection;
        int _growCount;
      public:
        Branch(Branch*,const L::Point3f&,const L::Point3f&);
        void reset();
        void addGrowth(L::Point3f);
        Branch next();
        inline const L::Point3f& position() const {return _position;}
        inline bool growing() const {return _growCount;}
    };
  private:
    L::Vector<Branch*> _branches;
    L::Vector<L::Point3f> _targets;
    float _minDist, _maxDist;
  public:
    SCA(float minDist, float maxDist);
    void addBranch(const Branch&);
    void addTarget(const L::Point3f&);
    bool update(World&);
};

#endif



