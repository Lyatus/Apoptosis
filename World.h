#ifndef DEF_Cancer_World
#define DEF_Cancer_World

#include <L/L.h>
#include "Chunk.h"
#include "shapes/Shape.h"

class World {
  public:
    static const int size = 64;
    static const int radius = size/2;
    static const int arraySize = size*size*size;
  private:
    Chunk* _chunks[size][size][size];
    Chunk **_min, **_max; // Pointers to the chunks with the minimum and maximum coordinates (used to optimize iteration)
  public:
    World();
    void draw();
    void update();
    Chunk& chunk(int x, int y, int z, bool create = true);
    const Voxel& voxel(int x, int y, int z, bool create = true);
    void updateVoxel(int x, int y, int z, const Voxel&, Voxel::Updater);

    bool raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance);

    // Drawing
    void fill(const Shape&, L::byte type, Voxel::Updater);
    void voxelSphere(L::Point3i center, float radius, L::byte type, Voxel::Updater = Voxel::set);

    static void chunkKey(int x, int y, int z, int& cx, int& cy, int& cz);
    static void voxelKey(int x, int y, int z, int& vx, int& vy, int& vz);
};

#endif



