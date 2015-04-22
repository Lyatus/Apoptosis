#include "World.h"

using namespace L;
using namespace GL;

World::World() {
  memset(_chunks,0,arraySize*sizeof(Chunk*));
  _min = _max = &_chunks[radius][radius][radius];
}
void World::draw(const Camera& camera) {
  int count(0);
  for(Chunk** c = _min; c<=_max; c++)
    if(*c && (*c)->draw(camera))
      count++;
  //std::cout << count << std::endl;
}
void World::update() {
  for(Chunk** c = _min; c<=_max; c++)
    if(*c)
      (*c)->update();
}
Chunk& World::chunk(int x, int y, int z, bool create) {
  Chunk*& chunk = _chunks[x+radius][y+radius][z+radius];
  if(chunk) return *chunk; // The chunk is already created
  else if(create) { // It isn't created and we're allowed to create it
    if(_max<&chunk) _max = &chunk;
    if(_min>&chunk) _min = &chunk;
    return *(chunk = new Chunk(x,y,z));
  } else throw Exception("Tried to access unavailable chunk.");
}
const Voxel& World::voxel(int x, int y, int z, bool create) {
  int cx, cy, cz, vx, vy, vz;
  chunkKey(x,y,z,cx,cy,cz);
  voxelKey(x,y,z,vx,vy,vz);
  return chunk(cx,cy,cz).voxel(vx,vy,vz);
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

bool World::raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance) {
  direction.normalize();
  hit = start;
  for(float i(0); i<distance; i+=.1f) {
    hit = start + direction*i;
    if(voxel(hit.x(),hit.y(),hit.z()).solid())
      return true;
  }
  return false;
}

void World::fill(const Shape& shape, L::byte type, Voxel::Updater u) {
  Set<Point3i> treated;
  if(shape.convex()) {
    struct Case {
      Point3i point;
      L::byte direction;
    };
    Vector<Case> treating;
    for(L::byte i(0); i<8; i++)
      treating.push_back({shape.start()+Point3i((i&0x1)?1:0,(i&0x2)?1:0,(i&0x4)?1:0),i});
    while(!treating.empty()) {
      Case c(treating.back());
      treating.pop_back();
      if(!treated.has(c.point)) { // Not yet treated
        treated.insert(c.point);
        float value(shape.value(c.point));
        if(value>0.0) { // In the shape
          updateVoxel(c.point.x(),c.point.y(),c.point.z(),Voxel(value,type),u);
          treating.push_back({c.point+Point3i((c.direction&0x1)?1:-1,0,0),c.direction});
          treating.push_back({c.point+Point3i(0,(c.direction&0x2)?1:-1,0),c.direction});
          treating.push_back({c.point+Point3i(0,0,(c.direction&0x4)?1:-1),c.direction});
        }
      }
    }
  } else {
    Vector<Point3i> treating;
    treating.push_back(shape.start());
    while(!treating.empty()) {
      Point3i p(treating.back());
      treating.pop_back();
      if(!treated.has(p)) { // Not yet treated
        treated.insert(p);
        float value(shape.value(p));
        if(value>0.0) { // In the shape
          updateVoxel(p.x(),p.y(),p.z(),Voxel(value,type),u);
          treating.push_back(p+Point3i(1,0,0));
          treating.push_back(p+Point3i(-1,0,0));
          treating.push_back(p+Point3i(0,1,0));
          treating.push_back(p+Point3i(0,-1,0));
          treating.push_back(p+Point3i(0,0,1));
          treating.push_back(p+Point3i(0,0,-1));
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
  if(x<0) x -= Chunk::size - 1;
  if(y<0) y -= Chunk::size - 1;
  if(z<0) z -= Chunk::size - 1;
  cx = x / Chunk::size;
  cy = y / Chunk::size;
  cz = z / Chunk::size;
}
void World::voxelKey(int x, int y, int z, int& vx, int& vy, int& vz) {
  vx = x%Chunk::size;
  vy = y%Chunk::size;
  vz = z%Chunk::size;
  if(vx<0) vx += Chunk::size;
  if(vy<0) vy += Chunk::size;
  if(vz<0) vz += Chunk::size;
}
