#ifndef DEF_Cancer_ChunkMesher
#define DEF_Cancer_ChunkMesher

#include <L/L.h>

#include "Chunk.h"

class ChunkMesher {
  private:
    static const L::byte bbl = 0;
    static const L::byte bbr = 1;
    static const L::byte btl = 2;
    static const L::byte btr = 3;
    static const L::byte fbl = 4;
    static const L::byte fbr = 5;
    static const L::byte ftl = 6;
    static const L::byte ftr = 7;
    static const L::byte backBottom = 0;
    static const L::byte backTop = 1;
    static const L::byte backLeft = 2;
    static const L::byte backRight = 3;
    static const L::byte frontBottom = 4;
    static const L::byte frontTop = 5;
    static const L::byte frontLeft = 6;
    static const L::byte frontRight = 7;
    static const L::byte bottomLeft = 8;
    static const L::byte bottomRight = 9;
    static const L::byte topLeft = 10;
    static const L::byte topRight = 11;
    static L::byte meshes[256][15];
    static bool init;
    int _edgeVertices[(Chunk::size+1)*(Chunk::size+1)*(Chunk::size+1)*3];
    L::GL::MeshBuilder _meshBuilder;

    void generateCase(bool cube[8],L::byte mesh[15],bool complementary = true);
    L::uint vertex(int x, int y, int z, L::byte edge, Voxel cell[8]);
    void color(L::byte edge, Voxel cell[8]);
    float edgeValue(Voxel, Voxel);
  public:
    ChunkMesher();
    void build(Chunk&);
    const L::GL::MeshBuilder& meshBuilder() const {return _meshBuilder;}
};

#endif



