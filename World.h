#ifndef DEF_Cancer_World
#define DEF_Cancer_World

#include <L/L.h>
#include "Chunk.h"

class World {
  private:
    L::Map<L::Point3i,Chunk*> _chunks;
  public:
    World();
    void draw();
    void update();
    Chunk& chunk(int x, int y, int z, bool create = true);
    const Voxel& voxel(int x, int y, int z, bool create = true);
    void updateVoxel(int x, int y, int z, const Voxel&, Voxel::Updater);

    bool raycast(L::Point3f start, L::Point3f direction, L::Point3f& hit, float distance);
    void voxelSphere(L::Point3i center, float radius, byte type, Voxel::Updater = Voxel::set);

    static void chunkKey(int x, int y, int z, int& cx, int& cy, int& cz);
    static void voxelKey(int x, int y, int z, int& vx, int& vy, int& vz);
};

#endif



