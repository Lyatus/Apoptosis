#ifndef DEF_Cancer_Chunk
#define DEF_Cancer_Chunk

#include <L/L.h>
#include "Voxel.h"

class ChunkMesher;
class Chunk {
  public:
    static const int size = 16;
  private:
    int _x, _y, _z;
    bool _changed;
    Voxel _voxels[size+1][size+1][size+1];
    int _typesCount[Voxel::types];
    L::GL::Mesh _mesh;
    L::Interval3f _box;
    static ChunkMesher _chunkMesher;
  public:
    Chunk(int x, int y, int z);
    bool update();
    bool draw(const L::GL::Camera&);
    const Voxel& voxel(int x, int y, int z);
    void voxel(int x, int y, int z,const Voxel&);
    inline int typeCount(L::byte type) const {return _typesCount[type];}
    inline L::Point3i position() const {return L::Point3i(_x*size,_y*size,_z*size);}

    void write(L::File&) const;
    void read(L::File&);
};

#endif




