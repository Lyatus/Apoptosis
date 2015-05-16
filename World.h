#ifndef DEF_Cancer_World
#define DEF_Cancer_World

#include <L/L.h>
#include "Chunk.h"
#include "shapes/Shape.h"

class World {
  public:
    static const int size = 256;
    static const int radius = size/2;
    static const int arraySize = size*size*size;
  private:
    Chunk* _chunks[size][size][size];
    L::Interval3i _interval;
  public:
    World();
    void foreachChunk(void (*)(Chunk*));
    void draw(const L::GL::Camera&);
    void update();
    int typeCount(L::byte type) const;
    Chunk& chunk(int x, int y, int z, bool create = true);
    const Voxel& voxel(int x, int y, int z, bool create = true);
    float valueAt(const L::Point3f&);
    void updateVoxel(int x, int y, int z, const Voxel&, Voxel::Updater);

    bool raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance);
    bool spherecast(L::Point3f center, float radius);

    // Drawing
    void fill(const Shape&, L::byte type, Voxel::Updater);
    void voxelSphere(L::Point3i center, float radius, L::byte type, Voxel::Updater = Voxel::set);

    void write(L::File&) const;
    void read(L::File&);

    static void chunkKey(int x, int y, int z, int& cx, int& cy, int& cz);
    static void voxelKey(int x, int y, int z, int& vx, int& vy, int& vz);
};

#endif



