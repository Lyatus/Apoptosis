#include "Chunk.h"

#include "ChunkMesher.h"

using namespace L;
using namespace GL;

ChunkMesher Chunk::_chunkMesher;

Chunk::Chunk(int x, int y, int z)
  : _x(x), _y(y), _z(z), _changed(true) {
}
bool Chunk::update() {
  if(_changed) {
    _changed = false;
    _chunkMesher.build(*this);
    L_Reconstruct(Mesh,_mesh,(_chunkMesher.meshBuilder()));
    return true;
  }
  return false;
}
void Chunk::draw() {
  glPushMatrix();
  glTranslatef(_x*size,_y*size,_z*size);
  _mesh.draw();
  glPopMatrix();
}
const Voxel& Chunk::voxel(int x, int y, int z) {
  return _voxels[x][y][z];
}
void Chunk::voxel(int x, int y, int z, const Voxel& v) {
  if(_voxels[x][y][z] != v)
    _changed = true;
  _voxels[x][y][z] = v;
}

void Chunk::write(File& file) const{
  file.write((char*)&_x,sizeof(int));
  file.write((char*)&_y,sizeof(int));
  file.write((char*)&_z,sizeof(int));
  file.write((char*)_voxels,sizeof(_voxels));
}
void Chunk::read(File& file){
  file.read((char*)_voxels,sizeof(_voxels));
  _changed = true;
}
