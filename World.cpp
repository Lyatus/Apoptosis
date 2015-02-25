#include "World.h"

using namespace L;
using namespace GL;

World::World() {}
void World::draw() {
  L_Iter(_chunks,chunk)
  chunk->second->draw();
}
void World::update() {
  L_Iter(_chunks,chunk)
  chunk->second->update();
}
Chunk& World::chunk(int x, int y, int z, bool create) {
  Point3i key(x,y,z);
  if(_chunks.has(key))
    return *_chunks[key];
  else if(create) {
    Chunk* chunk = new Chunk(x,y,z);
    return *(_chunks[key] = chunk);
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
  voxelChunk.setVoxel(vx,vy,vz,nv); // Set value
  // Check for possible neighbors being affected by the change
  if(!vx) chunk(cx-1,cy,cz).setVoxel(Chunk::size,vy,vz,nv);
  if(!vy) chunk(cx,cy-1,cz).setVoxel(vx,Chunk::size,vz,nv);
  if(!vx && !vy) chunk(cx-1,cy-1,cz).setVoxel(Chunk::size,Chunk::size,vz,nv);
  if(!vz) chunk(cx,cy,cz-1).setVoxel(vx,vy,Chunk::size,nv);
  if(!vx && !vz) chunk(cx-1,cy,cz-1).setVoxel(Chunk::size,vy,Chunk::size,nv);
  if(!vy && !vz) chunk(cx,cy-1,cz-1).setVoxel(vx,Chunk::size,Chunk::size,nv);
  if(!vx && !vy && !vz) chunk(cx-1,cy-1,cz-1).setVoxel(Chunk::size,Chunk::size,Chunk::size,nv);
}

bool World::raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance) {
  direction.normalize();
  hit = start;
  for(float i(0); i<distance; i+=.1f) {
    hit = start + direction*i;
    if(voxel(hit.x(),hit.y(),hit.z()).isSolid())
      return true;
  }
  return false;
}
void World::voxelSphere(L::Point3f center, float radius, Voxel::Updater u) {
  int r(radius+1);
  for(int x(-r); x<=r; x++)
    for(int y(-r); y<=r; y++)
      for(int z(-r); z<=r; z++)
        if(sqrt(x*x+y*y+z*z)<=radius+1)
          updateVoxel(x+center.x(),y+center.y(),z+center.z(),Voxel(std::min(1.0,std::max(0.0,1.0-(sqrt(x*x+y*y+z*z)-radius)))),u);
}

void World::chunkKey(int x, int y, int z, int& cx, int& cy, int& cz) {
  cx = ((x < 0) ? (x - Chunk::size + 1) : x) / Chunk::size;
  cy = ((y < 0) ? (y - Chunk::size + 1) : y) / Chunk::size;
  cz = ((z < 0) ? (z - Chunk::size + 1) : z) / Chunk::size;
}
void World::voxelKey(int x, int y, int z, int& vx, int& vy, int& vz) {
  vx = PMod(x,Chunk::size);
  vy = PMod(y,Chunk::size);
  vz = PMod(z,Chunk::size);
}
