#ifndef DEF_Cancer_ChunkMesher
#define DEF_Cancer_ChunkMesher

#include <L/L.h>

#include "Chunk.h"

class ChunkMesher {
  private:
    static const byte bbl = 0;
    static const byte bbr = 1;
    static const byte btl = 2;
    static const byte btr = 3;
    static const byte fbl = 4;
    static const byte fbr = 5;
    static const byte ftl = 6;
    static const byte ftr = 7;
    static const byte backBottom = 0;
    static const byte backTop = 1;
    static const byte backLeft = 2;
    static const byte backRight = 3;
    static const byte frontBottom = 4;
    static const byte frontTop = 5;
    static const byte frontLeft = 6;
    static const byte frontRight = 7;
    static const byte bottomLeft = 8;
    static const byte bottomRight = 9;
    static const byte topLeft = 10;
    static const byte topRight = 11;
    static byte meshes[256][15];
    static bool init;
    int _edgeVertices[(Chunk::size+1)*(Chunk::size+1)*(Chunk::size+1)*3];
    L::GL::MeshBuilder _meshBuilder;

    void generateCase(bool cube[8],byte mesh[15],bool complementary = true);
    L::uint vertex(int x, int y, int z, byte edge, Voxel cell[8]);
    void color(byte edge, Voxel cell[8]);
    float edgeValue(Voxel, Voxel);
  public:
    ChunkMesher();
    void build(Chunk&);
    const L::GL::MeshBuilder& meshBuilder() const {return _meshBuilder;}
};

#endif



