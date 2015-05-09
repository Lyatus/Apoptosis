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
        void addGrowth(const L::Point3f&);
        Branch next(World&);
        inline const L::Point3f& position() const {return _position;}
        inline const L::Point3f& originalDirection() const {return _originalDirection;}
        inline bool growing() const {return _growCount;}
    };
  private:
    L::Vector<Branch*> _branches;
    L::Octree<float,Branch*> _branchTree;
    L::Vector<L::Point3f> _targets;
    L::Timer _timer;
    float _minDist, _maxDist;
    bool _changed;
    static float dragFactor, randomFactor, branchLength, branchRadius;
    static int avoidAttempts;
  public:
    SCA(float minDist, float maxDist);
    ~SCA();
    void addBranch(const Branch&);
    void addTarget(const L::Point3f&);
    bool update(World&);
    Branch* nearest(const L::Point3f&, float maxDistance) const;
    float distance(const L::Point3f&, float maxDistance) const;

    static void configure();
};

#endif



