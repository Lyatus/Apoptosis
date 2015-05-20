#include "SCA.h"

#include "Conf.h"

using namespace L;

Color SCA::color;
float SCA::dragFactor, SCA::randomFactor, SCA::branchLength, SCA::branchRadius;
int SCA::avoidAttempts;

SCA::Branch::Branch(Branch* parent,const L::Point3f& position,const L::Point3f& direction)
  : _parent(parent), _position(position), _originalDirection(direction) {
  reset();
}
void SCA::Branch::reset() {
  _direction = _originalDirection*dragFactor;
  _growCount = 0;
}
void SCA::Branch::addGrowth(const L::Point3f& target) {
  Point3f direction(target-_position);
  direction.normalize();
  _direction += direction;
  _growCount++;
}
SCA::Branch SCA::Branch::next(World& world) {
  Point3f nextDirection, nextPosition, bestNextDirection, bestNextPosition;
  float bestValue;
  Voxel voxel;
  for(int i(0); i<avoidAttempts; i++) {
    nextDirection = (_direction/_growCount + Point3f::random()*randomFactor);
    nextDirection.normalize();
    nextDirection *= branchLength;
    nextPosition = _position+nextDirection;
    float value(world.valueAt(nextPosition));
    if(!i || abs(value-.5f)<abs(bestValue-.5f)) {
      bestNextDirection = nextDirection;
      bestNextPosition = nextPosition;
      bestValue = value;
    }
  }
  reset();
  return Branch(this,bestNextPosition,bestNextDirection);
}
void SCA::Branch::draw() {
  glBegin(GL_LINES);
  glVertex3f(_position.x(),_position.y(),_position.z());
  glVertex3f((_position-_originalDirection).x(),(_position-_originalDirection).y(),(_position-_originalDirection).z());
  glEnd();
}

SCA::SCA(float minDist, float maxDist)
  : _minDist(minDist), _maxDist(maxDist), _changed(false) {
}
SCA::~SCA() {
  for(int i(0); i<_branches.size(); i++)
    delete _branches[i];
}
void SCA::addBranch(const Branch& b) {
  Branch* branch(new Branch(b));
  _branches.push(branch);
  _branchTree.insert(b.position(),branch);
}
void SCA::addTarget(const Point3f& p) {
  _targets.push(p);
}
bool SCA::update(World& world) {
  bool addedBranch(false);
  for(int i(0); i<_targets.size(); i++) { // Process targets
    const Point3f& target(_targets[i]);
    Branch* nearestBranch(nearest(target,_maxDist));
    if(nearestBranch) {
      float nearestDistance(target.dist(nearestBranch->position()));
      if(nearestDistance<=_minDist || Rand::nextFloat()<.01f) // A branch is close enough to erase the target
        _targets.erase(i--);
      else // A branch is close enough to be affected by the target
        nearestBranch->addGrowth(target);
    }
  }
  for(int i(0); i<_branches.size(); i++) { // Add new branches
    SCA::Branch& branch(*_branches[i]);
    if(branch.growing()) { // There's at least one target that affects it
      Branch newBranch(branch.next(world));
      world.fill(Line(branch.position(),newBranch.position(),branchRadius),Voxel::VESSEL,Voxel::max);
      addBranch(newBranch);
      _changed = addedBranch = true;
    }
  }
  if(_changed && _timer.every(Time(0,0,1))) {
    _branchTree.balance();
    _changed = false;
  }
  return addedBranch;
}
SCA::Branch* SCA::nearest(const L::Point3f& point, float maxDistance) const {
  auto node(_branchTree.nearest(point,maxDistance));
  return (node)?node->value():NULL;
}
float SCA::distance(const Point3f& point, float maxDistance) const {
  SCA::Branch* nearest(nearest(point,maxDistance));
  return (nearest)?point.dist(nearest->position()):maxDistance;
}

void SCA::draw() {
  GL::color(color);
  for(int i(0); i<_branches.size(); i++)
    _branches[i]->draw();
}
void SCA::drawTargets() {
  GL::color(Color::red);
  glBegin(GL_POINTS);
  for(int i(0); i<_targets.size(); i++)
    glVertex3f(_targets[i].x(),_targets[i].y(),_targets[i].z());
  glEnd();
}

void SCA::configure() {
  dragFactor = Conf::getFloat("sca_drag_factor");
  randomFactor = Conf::getFloat("sca_random_factor");
  branchLength = Conf::getFloat("sca_branch_length");
  branchRadius = Conf::getFloat("sca_branch_radius");
  avoidAttempts = Conf::getInt("sca_avoid_attempts");
  color = Conf::getColor("sca_color");
}
