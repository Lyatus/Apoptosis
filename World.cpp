#include "World.h"

using namespace L;
using namespace GL;

World::World() {
  memset(_chunks,0,arraySize*sizeof(Chunk*));
}
void World::draw(const Camera& camera) {
  Point3i i(_interval.min());
  while(i.increment(_interval.min(),_interval.max())) {
    Chunk* chunk(_chunks[i.x()][i.y()][i.z()]);
    if(chunk)
      chunk->draw(camera);
  }
}
void World::update() {
  static Point3i i(_interval.min());
  int count(0);
  while(i.increment(_interval.min(),_interval.max())) {
    Chunk* chunk(_chunks[i.x()][i.y()][i.z()]);
    if(chunk && chunk->update() && ++count==8)
      return;
  }
}
void World::updateAll() {
  Point3i i(_interval.min());
  while(i.increment(_interval.min(),_interval.max())) {
    Chunk* chunk(_chunks[i.x()][i.y()][i.z()]);
    if(chunk) chunk->update();
  }
}
int World::typeCount(L::byte type) const {
  int wtr(0);
  Point3i i(_interval.min());
  while(i.increment(_interval.min(),_interval.max())) {
    Chunk* chunk(_chunks[i.x()][i.y()][i.z()]);
    if(chunk) wtr += chunk->typeCount(type);
  }
  return wtr;
}
Chunk& World::chunk(int x, int y, int z) {
  Chunk*& chunk = _chunks[x+radius][y+radius][z+radius];
  if(chunk) return *chunk; // The chunk is already created
  else { // It isn't created and we're allowed to create it
    _interval.add(Point3i(x+radius,y+radius,z+radius));
    _interval.add(Point3i(x+radius+1,y+radius+1,z+radius+1));
    return *(chunk = new Chunk(x,y,z));
  }
}
Chunk* World::chunkPointer(int x, int y, int z) {
  return _chunks[x+radius][y+radius][z+radius];
}
bool World::chunkExists(int x, int y, int z) {
  return chunkPointer(x,y,z);
}
const Voxel& World::voxel(int x, int y, int z) {
  static const Voxel empty;
  int cx, cy, cz;
  chunkKey(x,y,z,cx,cy,cz);
  Chunk* chunk(chunkPointer(cx,cy,cz));
  if(chunk) {
    int vx, vy, vz;
    voxelKey(x,y,z,vx,vy,vz);
    chunk->voxel(vx,vy,vz);
  } else return empty;
}
float World::valueAt(const Point3f& point) {
  int x(floor(point.x())), y(floor(point.y())), z(floor(point.z()));
  Point3f weight(PMod(point.x(),1.f),PMod(point.y(),1.f),PMod(point.z(),1.f));
  float cell[8] = {
    voxel(x,y,z).value(),
    voxel(x,y,z+1).value(),
    voxel(x,y+1,z).value(),
    voxel(x,y+1,z+1).value(),
    voxel(x+1,y,z).value(),
    voxel(x+1,y,z+1).value(),
    voxel(x+1,y+1,z).value(),
    voxel(x+1,y+1,z+1).value()
  };
  return Interpolation<3,float>::linear(cell,weight);
}
void World::updateVoxel(int x, int y, int z, const Voxel& v, Voxel::Updater u) {
  int cx, cy, cz, vx, vy, vz;
  chunkKey(x,y,z,cx,cy,cz);
  voxelKey(x,y,z,vx,vy,vz);
  Chunk& voxelChunk(chunk(cx,cy,cz)); // Chunk where the main instance of the voxel is
  Voxel nv(u(voxelChunk.voxel(vx,vy,vz),v)); // Compute new voxel value
  voxelChunk.voxel(vx,vy,vz,nv); // Set value
  // Check for possible neighbors being affected by the change
  if(!vx) chunk(cx-1,cy,cz).voxel(Chunk::size,vy,vz,nv);
  if(!vy) chunk(cx,cy-1,cz).voxel(vx,Chunk::size,vz,nv);
  if(!vx && !vy) chunk(cx-1,cy-1,cz).voxel(Chunk::size,Chunk::size,vz,nv);
  if(!vz) chunk(cx,cy,cz-1).voxel(vx,vy,Chunk::size,nv);
  if(!vx && !vz) chunk(cx-1,cy,cz-1).voxel(Chunk::size,vy,Chunk::size,nv);
  if(!vy && !vz) chunk(cx,cy-1,cz-1).voxel(vx,Chunk::size,Chunk::size,nv);
  if(!vx && !vy && !vz) chunk(cx-1,cy-1,cz-1).voxel(Chunk::size,Chunk::size,Chunk::size,nv);
}
void World::setVoxel(int x, int y, int z, const Voxel& v) {
  int cx, cy, cz, vx, vy, vz;
  chunkKey(x,y,z,cx,cy,cz);
  voxelKey(x,y,z,vx,vy,vz);
  Chunk& voxelChunk(chunk(cx,cy,cz)); // Chunk where the main instance of the voxel is
  voxelChunk.voxel(vx,vy,vz,v); // Set value
  // Check for possible neighbors being affected by the change
  if(!vx) chunk(cx-1,cy,cz).voxel(Chunk::size,vy,vz,v);
  if(!vy) chunk(cx,cy-1,cz).voxel(vx,Chunk::size,vz,v);
  if(!vx && !vy) chunk(cx-1,cy-1,cz).voxel(Chunk::size,Chunk::size,vz,v);
  if(!vz) chunk(cx,cy,cz-1).voxel(vx,vy,Chunk::size,v);
  if(!vx && !vz) chunk(cx-1,cy,cz-1).voxel(Chunk::size,vy,Chunk::size,v);
  if(!vy && !vz) chunk(cx,cy-1,cz-1).voxel(vx,Chunk::size,Chunk::size,v);
  if(!vx && !vy && !vz) chunk(cx-1,cy-1,cz-1).voxel(Chunk::size,Chunk::size,Chunk::size,v);
}

bool World::raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance) {
  direction.normalize();
  hit = start;
  for(float i(0); i<distance; i+=.5f) {
    hit = start + direction*i;
    if(voxel(hit.x(),hit.y(),hit.z()).solid())
      return true;
  }
  return false;
}
bool World::spherecast(L::Point3f center, float radius) {
  for(int i(0); i<6; i++) {
    Point3f tmp(center);
    tmp[i%3] += ((i<3)?radius:-radius);
    if(voxel(tmp.x(),tmp.y(),tmp.z()).solid())
      return true;
  }
  return voxel(center.x(),center.y(),center.z()).solid();
}
bool World::spherecast(L::Point3f center, float radius, const std::function<bool(Voxel)>& f) {
  for(int i(0); i<6; i++) {
    Point3f tmp(center);
    tmp[i%3] += ((i<3)?radius:-radius);
    if(f(voxel(tmp.x(),tmp.y(),tmp.z())))
      return true;
  }
  return f(voxel(center.x(),center.y(),center.z()));
}
void World::fill(const Shape& shape, L::byte type, Voxel::Updater u) {
  Set<Point3i> treated;
  if(shape.convex()) {
    struct Case {
      Point3i point;
      L::byte direction;
    };
    Array<Case> treating;
    for(L::byte i(0); i<8; i++)
      treating.push<Case>({shape.start()+Point3i((i&0x1)?1:0,(i&0x2)?1:0,(i&0x4)?1:0),i});
    while(!treating.empty()) {
      Case c(treating.back());
      treating.pop();
      if(!treated.has(c.point)) { // Not yet treated
        treated.insert(c.point);
        float value(shape.value(c.point));
        if(value>0.0) { // In the shape
          updateVoxel(c.point.x(),c.point.y(),c.point.z(),Voxel(value,type),u);
          treating.push<Case>({c.point+Point3i((c.direction&0x1)?1:-1,0,0),c.direction});
          treating.push<Case>({c.point+Point3i(0,(c.direction&0x2)?1:-1,0),c.direction});
          treating.push<Case>({c.point+Point3i(0,0,(c.direction&0x4)?1:-1),c.direction});
        }
      }
    }
  } else {
    Array<Point3i> treating;
    treating.push(shape.start());
    while(!treating.empty()) {
      Point3i p(treating.back());
      treating.pop();
      if(!treated.has(p)) { // Not yet treated
        treated.insert(p);
        float value(shape.value(p));
        if(value>0.0) { // In the shape
          updateVoxel(p.x(),p.y(),p.z(),Voxel(value,type),u);
          treating.push(p+Point3i(1,0,0));
          treating.push(p+Point3i(-1,0,0));
          treating.push(p+Point3i(0,1,0));
          treating.push(p+Point3i(0,-1,0));
          treating.push(p+Point3i(0,0,1));
          treating.push(p+Point3i(0,0,-1));
        }
      }
    }
  }
}
void World::voxelSphere(Point3i center, float radius, L::byte type, Voxel::Updater u) {
  int r((int)radius+1);
  for(int x(-r); x<=r; x++)
    for(int y(-r); y<=r; y++)
      for(int z(-r); z<=r; z++)
        if(sqrt(x*x+y*y+z*z)<=radius+1)
          updateVoxel(x+center.x(),y+center.y(),z+center.z(),Voxel(std::min(1.f,std::max(0.f,1.f-((float)sqrt(x*x+y*y+z*z)-radius))),type),u);
}

void World::write(File& file) const {
  for(int x(0); x<size; x++)
    for(int y(0); y<size; y++)
      for(int z(0); z<size; z++)
        if(_chunks[x][y][z])
          _chunks[x][y][z]->write(file);
}
void World::read(File& file) {
  while(true) {
    int x, y, z;
    if(file.read((char*)&x,sizeof(int))<sizeof(int)) break;
    if(file.read((char*)&y,sizeof(int))<sizeof(int)) break;
    if(file.read((char*)&z,sizeof(int))<sizeof(int)) break;
    chunk(x,y,z).read(file);
  }
}

void World::chunkKey(int x, int y, int z, int& cx, int& cy, int& cz) {
  cx = x >> Chunk::sizeBits;
  cy = y >> Chunk::sizeBits;
  cz = z >> Chunk::sizeBits;
}
void World::voxelKey(int x, int y, int z, int& vx, int& vy, int& vz) {
  vx = x & Chunk::sizeMask;
  vy = y & Chunk::sizeMask;
  vz = z & Chunk::sizeMask;
}
