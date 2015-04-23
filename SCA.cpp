#include "SCA.h"

using namespace L;
using namespace GL;

SCA::Branch::Branch(Branch* parent,const L::Point3f& position,const L::Point3f& direction)
  : _parent(parent), _position(position), _originalDirection(direction) {
  reset();
}
void SCA::Branch::reset() {
  _direction = _originalDirection*3;
  _growCount = 0;
}
void SCA::Branch::addGrowth(L::Point3f direction) {
  direction.normalize();
  _direction += direction;
  _growCount++;
}
SCA::Branch SCA::Branch::next() {
  Point3f avgDirection(_direction/_growCount);
  avgDirection.normalize();
  reset();
  return Branch(this,(_position+avgDirection*1),avgDirection);
}

SCA::SCA(float minDist, float maxDist)
  : _minDist(minDist), _maxDist(maxDist) {
}
void SCA::addBranch(const Branch& b) {
  _branches.push_back(new Branch(b));
}
void SCA::addTarget(const Point3f& p) {
  _targets.push_back(p);
}
bool SCA::update(World& world) {
  bool addedBranch(false);
  for(int i(0); i<_targets.size(); i++) { // Process targets
    const Point3f& target(_targets[i]);
    Branch* closestBranch(NULL);
    float closestDistance(std::numeric_limits<float>::max());
    for(int j(0); j<_branches.size(); j++) { // Find the closest branch to this target
      SCA::Branch& branch(*_branches[j]);
      float distance(branch.position().dist(target));
      if(distance<_maxDist && distance<closestDistance) {
        closestBranch = &branch;
        closestDistance = distance;
      }
    }
    if(closestDistance<_minDist) // A branch is close enough to erase the target
      _targets.erase(_targets.begin()+i--);
    else if(closestBranch)  // A branch is close enough to be affected by the target
      closestBranch->addGrowth(target-closestBranch->position());
  }
  for(int i(0); i<_branches.size(); i++) { // Add new branches
    SCA::Branch& branch(*_branches[i]);
    if(branch.growing()) { // There's at least one target that affects it
      Branch newBranch(branch.next());
      world.fill(Line(branch.position(),newBranch.position(),1),Voxel::VESSEL,Voxel::max);
      addBranch(newBranch);
      addedBranch = true;
    }
  }
  return addedBranch;
}

