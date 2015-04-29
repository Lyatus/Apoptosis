#include "SCA.h"

#include "Conf.h"

using namespace L;

float SCA::randomFactor;

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
  Point3f avgDirection(_direction/_growCount + Point3f::random()*randomFactor);
  avgDirection.normalize();
  reset();
  return Branch(this,(_position+avgDirection*1),avgDirection);
}

SCA::SCA(float minDist, float maxDist)
  : _minDist(minDist), _maxDist(maxDist) {
}
SCA::~SCA() {
  L_Iter(_branches,it)
  delete *it;
}
void SCA::addBranch(const Branch& b) {
  Branch* branch(new Branch(b));
  _branches.push_back(branch);
  _branchTree.insert(b.position(),branch);
}
void SCA::addTarget(const Point3f& p) {
  _targets.push_back(p);
}
bool SCA::update(World& world) {
  bool addedBranch(false);
  for(int i(0); i<_targets.size(); i++) { // Process targets
    const Point3f& target(_targets[i]);
    Branch* nearestBranch(nearest(target,_maxDist));
    if(nearestBranch) {
      float nearestDistance(target.dist(nearestBranch->position()));
      if(nearestDistance<_minDist) // A branch is close enough to erase the target
        _targets.erase(_targets.begin()+i--);
      else // A branch is close enough to be affected by the target
        nearestBranch->addGrowth(target-nearestBranch->position());
    }
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
SCA::Branch* SCA::nearest(const L::Point3f& point, float maxDistance) const {
  auto node(_branchTree.nearest(point));
  return (node)?node->value():NULL;
}
float SCA::distance(const Point3f& point, float maxDistance) const {
  SCA::Branch* nearest(nearest(point,maxDistance));
  return (nearest)?point.dist(nearest->position()):maxDistance;
}

void SCA::configure() {
  randomFactor = Conf::getFloat("sca_random_factor");
}
