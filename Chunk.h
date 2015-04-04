#ifndef DEF_Cancer_Chunk
#define DEF_Cancer_Chunk

#include <L/L.h>
#include "Voxel.h"

class ChunkMesher;
class Chunk {
  public:
    static const int size = 24;
  private:
    int _x, _y, _z;
    bool _changed;
    Voxel _voxels[size+1][size+1][size+1];
    L::GL::Mesh _mesh;
    static ChunkMesher _chunkMesher;
  public:
    Chunk(int x, int y, int z);
    bool update();
    void draw();
    const Voxel& voxel(int x, int y, int z);
    void voxel(int x, int y, int z,const Voxel&);

    void write(L::File&) const;
    void read(L::File&);
};

#endif



