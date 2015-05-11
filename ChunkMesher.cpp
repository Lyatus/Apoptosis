#include "ChunkMesher.h"

#include "Chunk.h"

using namespace L;
using namespace GL;

L::byte ChunkMesher::meshes[256][15] = {
  {255,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,8,2,255,0,0,0,0,0,0,0,0,0,0,0},
  {3,9,0,255,0,0,0,0,0,0,0,0,0,0,0},
  {2,9,8,9,2,3,255,0,0,0,0,0,0,0,0},
  {2,10,1,255,0,0,0,0,0,0,0,0,0,0,0},
  {1,8,10,8,1,0,255,0,0,0,0,0,0,0,0},
  {3,9,0,1,2,10,255,0,0,0,0,0,0,0,0},
  {9,8,10,9,1,3,1,9,10,255,0,0,0,0,0},
  {1,11,3,255,0,0,0,0,0,0,0,0,0,0,0},
  {0,8,2,3,1,11,255,0,0,0,0,0,0,0,0},
  {0,11,9,11,0,1,255,0,0,0,0,0,0,0,0},
  {11,9,8,11,2,1,2,11,8,255,0,0,0,0,0},
  {3,10,11,10,3,2,255,0,0,0,0,0,0,0,0},
  {8,10,11,8,3,0,3,8,11,255,0,0,0,0,0},
  {10,11,9,10,0,2,0,10,9,255,0,0,0,0,0},
  {9,10,11,10,9,8,255,0,0,0,0,0,0,0,0},
  {8,4,6,255,0,0,0,0,0,0,0,0,0,0,0},
  {6,0,4,0,6,2,255,0,0,0,0,0,0,0,0},
  {0,3,9,8,4,6,255,0,0,0,0,0,0,0,0},
  {6,2,3,6,9,4,9,6,3,255,0,0,0,0,0},
  {8,4,6,2,10,1,255,0,0,0,0,0,0,0,0},
  {1,0,4,1,6,10,6,1,4,255,0,0,0,0,0},
  {1,10,2,3,0,9,6,4,8,255,0,0,0,0,0},
  {9,1,3,9,10,1,10,9,4,4,6,10,255,0,0},
  {1,11,3,6,8,4,255,0,0,0,0,0,0,0,0},{6,0,4,0,6,2,3,1,11,255,0,0,0,0,0},{11,0,1,0,11,9,8,4,6,255,0,0,0,0,0},{9,4,11,4,2,11,2,1,11,4,6,2,255,0,0},{3,10,11,10,3,2,6,8,4,255,0,0,0,0,0},{
    4,6,0,6,3,0
    ,3,6,11,6,10,11,255,0,0
  },{2,10,0,0,10,9,10,11,9,8,4,6,255,0,0},{
    10,9,6,4,6,9,10,
    11,9,255,0,0,0,0,0
  },{9,7,4,255,0,0,0,0,0,0,0,0,0,0,0},{
    9,7,4,0,8,2,255,0,0,0,0,0
    ,0,0,0
  },{4,3,7,3,4,0,255,0,0,0,0,0,0,0,0},{2,3,7,2,4,8,4,2,7,255,0,0,0,0,0},{2,10,1,4,9,7,255,0,0,0,0,0,0,0,0},{1,8,10,8,1,0,4,9,7,255,0,0,0,0,0},{
    4,3,7,3,4,0,1
    ,2,10,255,0,0,0,0,0
  },{7,4,3,4,1,3,1,4,10,4,8,10,255,0,0},{
    3,1,11,9,7,4,255,0,0,0
    ,0,0,0,0,0
  },{2,8,0,1,3,11,4,7,9,255,0,0,0,0,0},{
    4,0,1,4,11,7,11,4,1,255,0,0,0,0,
    0
  },{11,2,1,11,8,2,8,11,7,7,4,8,255,0,0},{10,3,2,3,10,11,9,7,4,255,0,0,0,0,0},{
    0,
    8,3,3,8,11,8,10,11,9,7,4,255,0,0
  },{11,7,10,7,0,10,0,2,10,7,4,0,255,0,0},{
    8,11,4,
    7,4,11,8,10,11,255,0,0,0,0,0
  },{7,8,9,8,7,6,255,0,0,0,0,0,0,0,0},{
    7,6,2,7,0,9,0,7
    ,2,255,0,0,0,0,0
  },{3,7,6,3,8,0,8,3,6,255,0,0,0,0,0},{
    3,6,2,6,3,7,255,0,0,0,0,0,0
    ,0,0
  },{7,8,9,8,7,6,2,10,1,255,0,0,0,0,0},{6,10,7,10,0,7,0,9,7,10,1,0,255,0,0},{
    0
    ,3,8,8,3,6,3,7,6,2,10,1,255,0,0
  },{3,6,1,10,1,6,3,7,6,255,0,0,0,0,0},{
    8,7,6,7,8,9
    ,11,3,1,255,0,0,0,0,0
  },{9,7,0,0,7,2,7,6,2,3,1,11,255,0,0},{
    6,8,7,8,11,7,11,8,1,8
    ,0,1,255,0,0
  },{7,2,11,1,11,2,7,6,2,255,0,0,0,0,0},{3,10,11,10,3,2,7,8,9,8,7,6,255,0,0},{9,7,0,7,6,0,6,10,0,10,11,0,11,3,0},{2,10,0,10,11,0,11,7,0,7,6,0,6,8,0},{
    7,6,11,10,11,6,255,0,0,0,0,0,0,0,0
  },{10,6,5,255,0,0,0,0,0,0,0,0,0,0,0},{
    2,0,8,10
    ,6,5,255,0,0,0,0,0,0,0,0
  },{3,9,0,5,10,6,255,0,0,0,0,0,0,0,0},{
    9,2,3,2,9,8,10,6,5
    ,255,0,0,0,0,0
  },{5,2,6,2,5,1,255,0,0,0,0,0,0,0,0},{
    5,1,0,5,8,6,8,5,0,255,0,0,0,0
    ,0
  },{5,2,6,2,5,1,0,3,9,255,0,0,0,0,0},{8,6,9,6,1,9,1,3,9,6,5,1,255,0,0},{
    10,6,5,
    1,11,3,255,0,0,0,0,0,0,0,0
  },{3,11,1,0,2,8,5,6,10,255,0,0,0,0,0},{
    0,11,9,11,0,1,5
    ,10,6,255,0,0,0,0,0
  },{1,11,2,2,11,8,11,9,8,10,6,5,255,0,0},{3,2,6,3,5,11,5,3,6,255,0,0,0,0,0},{8,3,0,8,11,3,11,8,6,6,5,11,255,0,0},{
    6,5,2,5,0,2,0,5,9,5,11,9,255
    ,0,0
  },{11,8,5,6,5,8,11,9,8,255,0,0,0,0,0},{4,10,8,10,4,5,255,0,0,0,0,0,0,0,0},{
    0
    ,4,5,0,10,2,10,0,5,255,0,0,0,0,0
  },{10,4,5,4,10,8,9,0,3,255,0,0,0,0,0},{
    5,10,4,10
    ,9,4,9,10,3,10,2,3,255,0,0
  },{4,5,1,4,2,8,2,4,1,255,0,0,0,0,0},{
    0,5,1,5,0,4,255,0
    ,0,0,0,0,0,0,0
  },{8,4,2,2,4,1,4,5,1,0,3,9,255,0,0},{
    4,1,9,3,9,1,4,5,1,255,0,0,0,0
    ,0
  },{4,10,8,10,4,5,1,11,3,255,0,0,0,0,0},{2,0,10,10,0,5,0,4,5,1,11,3,255,0,0},{
    0
    ,11,9,11,0,1,4,10,8,10,4,5,255,0,0
  },{1,11,2,11,9,2,9,4,2,4,5,2,5,10,2},{5,11,4,11,2,4,2,8,4,11,3,2,255,0,0},{0,5,3,11,3,5,0,4,5,255,0,0,0,0,0},{
    8,4,2,4,5,2,5,11
    ,2,11,9,2,9,0,2
  },{4,5,9,11,9,5,255,0,0,0,0,0,0,0,0},{
    4,9,7,6,5,10,255,0,0,0,0,0,
    0,0,0
  },{7,9,4,5,6,10,0,2,8,255,0,0,0,0,0},{3,4,0,4,3,7,6,5,10,255,0,0,0,0,0},{
    8,
    2,4,4,2,7,2,3,7,6,5,10,255,0,0
  },{2,5,1,5,2,6,7,4,9,255,0,0,0,0,0},{
    6,5,8,8,5,0,5
    ,1,0,4,9,7,255,0,0
  },{4,3,7,3,4,0,5,2,6,2,5,1,255,0,0},{
    6,5,8,5,1,8,1,3,8,3,7,8,7
    ,4,8
  },{6,10,5,4,7,9,1,3,11,255,0,0,0,0,0},{0,8,2,3,1,11,4,9,7,6,5,10,255,0,0},{
    7
    ,4,11,11,4,1,4,0,1,5,10,6,255,0,0
  },{4,11,7,4,1,11,4,8,1,8,2,1,7,5,10},{
    11,3,5,5,
    3,6,3,2,6,7,4,9,255,0,0
  },{5,8,6,5,0,8,5,11,0,11,3,0,6,4,9},{
    7,4,11,4,0,11,0,2,11
    ,2,6,11,6,5,11
  },{6,11,8,11,6,5,7,8,11,8,7,4,255,0,0},{
    10,8,9,10,7,5,7,10,9,255,0
    ,0,0,0,0
  },{0,10,2,0,5,10,5,0,9,9,7,5,255,0,0},{
    8,0,10,0,7,10,7,5,10,0,3,7,255,0,
    0
  },{2,7,10,5,10,7,2,3,7,255,0,0,0,0,0},{1,2,5,2,7,5,7,2,9,2,8,9,255,0,0},{
    5,0,7,
    9,7,0,5,1,0,255,0,0,0,0,0
  },{0,3,8,3,7,8,7,5,8,5,1,8,1,2,8},{
    3,7,1,5,1,7,255,0,0,
    0,0,0,0,0,0
  },{5,10,7,7,10,9,10,8,9,11,3,1,255,0,0},{
    0,10,2,0,5,10,0,9,5,9,7,5,2,
    1,11
  },{5,10,7,10,8,7,8,0,7,0,1,7,1,11,7},{5,2,7,2,5,10,1,7,2,7,1,11,255,0,0},{
    11
    ,3,5,3,2,5,2,8,5,8,9,5,9,7,5
  },{9,5,0,5,9,7,11,0,5,0,11,3,255,0,0},{
    5,11,7,2,8,0,
    255,0,0,0,0,0,0,0,0
  },{7,5,11,255,0,0,0,0,0,0,0,0,0,0,0},{
    11,5,7,255,0,0,0,0,0,0,
    0,0,0,0,0
  },{0,8,2,7,11,5,255,0,0,0,0,0,0,0,0},{11,5,7,3,9,0,255,0,0,0,0,0,0,0,0}
  ,{2,9,8,9,2,3,7,11,5,255,0,0,0,0,0},{1,2,10,11,5,7,255,0,0,0,0,0,0,0,0},{
    8,1,0,1
    ,8,10,11,5,7,255,0,0,0,0,0
  },{0,9,3,2,1,10,7,5,11,255,0,0,0,0,0},{
    3,9,1,1,9,10,9,
    8,10,11,5,7,255,0,0
  },{7,1,5,1,7,3,255,0,0,0,0,0,0,0,0},{
    7,1,5,1,7,3,2,0,8,255,0,
    0,0,0,0
  },{0,1,5,0,7,9,7,0,5,255,0,0,0,0,0},{5,7,1,7,2,1,2,7,8,7,9,8,255,0,0},{
    7,
    3,2,7,10,5,10,7,2,255,0,0,0,0,0
  },{10,5,8,5,3,8,3,0,8,5,7,3,255,0,0},{
    10,0,2,10,9
    ,0,9,10,5,5,7,9,255,0,0
  },{9,10,7,5,7,10,9,8,10,255,0,0,0,0,0},{
    7,11,5,4,6,8,255,
    0,0,0,0,0,0,0,0
  },{0,6,2,6,0,4,5,7,11,255,0,0,0,0,0},{
    5,11,7,6,4,8,3,0,9,255,0,0,
    0,0,0
  },{4,6,9,9,6,3,6,2,3,7,11,5,255,0,0},{4,8,6,7,5,11,2,1,10,255,0,0,0,0,0},{10,1,6,6,1,4,1,0,4,5,7,11,255,0,0},{3,9,0,1,2,10,7,11,5,4,6,8,255,0,0},{
    6,9,4,6,3
    ,9,6,10,3,10,1,3,4,7,11
  },{1,7,3,7,1,5,4,6,8,255,0,0,0,0,0},{
    6,0,4,0,6,2,7,1,5,1,
    7,3,255,0,0
  },{9,0,7,7,0,5,0,1,5,4,6,8,255,0,0},{4,6,9,6,2,9,2,1,9,1,5,9,5,7,9},{
    5,7,10,10,7,2,7,3,2,6,8,4,255,0,0
  },{5,7,10,7,3,10,3,0,10,0,4,10,4,6,10},{
    7,10,5,
    7,2,10,7,9,2,9,0,2,5,6,8
  },{5,9,10,9,5,7,4,10,9,10,4,6,255,0,0},{
    5,9,11,9,5,4,255
    ,0,0,0,0,0,0,0,0
  },{5,9,11,9,5,4,0,8,2,255,0,0,0,0,0},{
    5,4,0,5,3,11,3,5,0,255,0,0
    ,0,0,0
  },{4,8,5,8,3,5,3,11,5,8,2,3,255,0,0},{9,5,4,5,9,11,10,1,2,255,0,0,0,0,0},{
    1,8,10,8,1,0,5,9,11,9,5,4,255,0,0
  },{11,5,3,3,5,0,5,4,0,1,2,10,255,0,0},{
    11,5,3,5
    ,4,3,4,8,3,8,10,3,10,1,3
  },{1,5,4,1,9,3,9,1,4,255,0,0,0,0,0},{
    3,1,9,9,1,4,1,5,4,0
    ,8,2,255,0,0
  },{1,4,0,4,1,5,255,0,0,0,0,0,0,0,0},{
    1,4,2,8,2,4,1,5,4,255,0,0,0,0,0
  },{4,9,5,9,10,5,10,9,2,9,3,2,255,0,0},{0,8,3,8,10,3,10,5,3,5,4,3,4,9,3},{
    5,0,10,
    2,10,0,5,4,0,255,0,0,0,0,0
  },{5,4,10,8,10,4,255,0,0,0,0,0,0,0,0},{
    8,9,11,8,5,6,5,
    8,11,255,0,0,0,0,0
  },{2,0,6,0,5,6,5,0,11,0,9,11,255,0,0},{
    3,8,0,3,6,8,6,3,11,11,5
    ,6,255,0,0
  },{6,3,5,11,5,3,6,2,3,255,0,0,0,0,0},{
    6,8,5,5,8,11,8,9,11,10,1,2,255,0
    ,0
  },{10,1,6,1,0,6,0,9,6,9,11,6,11,5,6},{3,8,0,3,6,8,3,11,6,11,5,6,0,2,10},{
    11,6,
    3,6,11,5,10,3,6,3,10,1,255,0,0
  },{9,3,8,3,5,8,5,6,8,3,1,5,255,0,0},{
    3,1,9,1,5,9,5
    ,6,9,6,2,9,2,0,9
  },{0,5,8,6,8,5,0,1,5,255,0,0,0,0,0},{
    1,5,2,6,2,5,255,0,0,0,0,0,0
    ,0,0
  },{6,8,5,8,9,5,9,3,5,3,2,5,2,10,5},{6,10,5,0,9,3,255,0,0,0,0,0,0,0,0},{
    6,0,5
    ,0,6,8,2,5,0,5,2,10,255,0,0
  },{5,6,10,255,0,0,0,0,0,0,0,0,0,0,0},{
    6,11,10,11,6,7,
    255,0,0,0,0,0,0,0,0
  },{11,6,7,6,11,10,8,2,0,255,0,0,0,0,0},{
    6,11,10,11,6,7,3,9,0,
    255,0,0,0,0,0
  },{2,9,8,9,2,3,6,11,10,11,6,7,255,0,0},{
    2,6,7,2,11,1,11,2,7,255,0,0
    ,0,0,0
  },{7,11,6,11,8,6,8,11,0,11,1,0,255,0,0},{
    1,2,11,11,2,7,2,6,7,3,9,0,255,0,0
  },{3,9,1,9,8,1,8,6,1,6,7,1,7,11,1},{6,7,3,6,1,10,1,6,3,255,0,0,0,0,0},{
    10,6,1,1,
    6,3,6,7,3,2,0,8,255,0,0
  },{7,9,6,9,1,6,1,10,6,9,0,1,255,0,0},{
    10,6,1,6,7,1,7,9,1,
    9,8,1,8,2,1
  },{2,7,3,7,2,6,255,0,0,0,0,0,0,0,0},{6,3,8,0,8,3,6,7,3,255,0,0,0,0,0}
  ,{2,7,0,9,0,7,2,6,7,255,0,0,0,0,0},{6,7,8,9,8,7,255,0,0,0,0,0,0,0,0},{
    11,10,8,11
    ,4,7,4,11,8,255,0,0,0,0,0
  },{10,2,11,2,4,11,4,7,11,2,0,4,255,0,0},{
    7,11,4,4,11,8,
    11,10,8,9,0,3,255,0,0
  },{7,11,4,11,10,4,10,2,4,2,3,4,3,9,4},{
    2,11,1,2,7,11,7,2,8,
    8,4,7,255,0,0
  },{1,4,11,7,11,4,1,0,4,255,0,0,0,0,0},{
    2,11,1,2,7,11,2,8,7,8,4,7,1,
    3,9
  },{7,1,4,1,7,11,3,4,1,4,3,9,255,0,0},{3,1,7,1,4,7,4,1,8,1,10,8,255,0,0},{
    2,0,
    10,0,4,10,4,7,10,7,3,10,3,1,10
  },{9,0,7,0,1,7,1,10,7,10,8,7,8,4,7},{
    7,9,4,1,10,2,
    255,0,0,0,0,0,0,0,0
  },{7,2,4,8,4,2,7,3,2,255,0,0,0,0,0},{
    0,4,3,7,3,4,255,0,0,0,0,
    0,0,0,0
  },{8,7,2,7,8,4,9,2,7,2,9,0,255,0,0},{4,7,9,255,0,0,0,0,0,0,0,0,0,0,0},{
    9,
    11,10,9,6,4,6,9,10,255,0,0,0,0,0
  },{4,9,6,6,9,10,9,11,10,8,2,0,255,0,0},{
    0,3,4,3,
    6,4,6,3,10,3,11,10,255,0,0
  },{8,2,4,2,3,4,3,11,4,11,10,4,10,6,4},{
    11,1,9,1,6,9,6,
    4,9,1,2,6,255,0,0
  },{4,9,6,9,11,6,11,1,6,1,0,6,0,8,6},{1,2,11,2,6,11,6,4,11,4,0,11,0,3,11},{4,8,6,3,11,1,255,0,0,0,0,0,0,0,0},{1,9,3,1,4,9,4,1,10,10,6,4,255,0,0}
  ,{1,9,3,1,4,9,1,10,4,10,6,4,3,0,8},{4,1,6,10,6,1,4,0,1,255,0,0,0,0,0},{
    10,4,1,4,
    10,6,8,1,4,1,8,2,255,0,0
  },{3,6,9,4,9,6,3,2,6,255,0,0,0,0,0},{
    4,3,6,3,4,9,0,6,3,6
    ,0,8,255,0,0
  },{2,6,0,4,0,6,255,0,0,0,0,0,0,0,0},{
    6,4,8,255,0,0,0,0,0,0,0,0,0,0,0
  },{8,11,10,11,8,9,255,0,0,0,0,0,0,0,0},{9,10,0,2,0,10,9,11,10,255,0,0,0,0,0},{
    11
    ,8,3,0,3,8,11,10,8,255,0,0,0,0,0
  },{2,3,10,11,10,3,255,0,0,0,0,0,0,0,0},{
    8,11,2,1
    ,2,11,8,9,11,255,0,0,0,0,0
  },{1,0,11,9,11,0,255,0,0,0,0,0,0,0,0},{
    1,8,11,8,1,2,0,
    11,8,11,0,3,255,0,0
  },{3,11,1,255,0,0,0,0,0,0,0,0,0,0,0},{
    10,9,1,3,1,9,10,8,9,255
    ,0,0,0,0,0
  },{2,9,10,9,2,0,3,10,9,10,3,1,255,0,0},{
    0,1,8,10,8,1,255,0,0,0,0,0,0,0
    ,0
  },{1,10,2,255,0,0,0,0,0,0,0,0,0,0,0},{3,2,9,8,9,2,255,0,0,0,0,0,0,0,0},{
    0,9,3,
    255,0,0,0,0,0,0,0,0,0,0,0
  },{2,8,0,255,0,0,0,0,0,0,0,0,0,0,0},{
    255,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0
  }
};

L::byte rotateCornersX[] = {4,5,0,1,6,7,2,3};
L::byte rotateCornersY[] = {1,5,3,7,0,4,2,6};
L::byte rotateCornersZ[] = {2,0,3,1,6,4,7,5};
L::byte rotateEdgesX[] = {1,5,10,11,0,4,8,9,2,3,6,7};
L::byte rotateEdgesY[] = {8,10,6,2,9,11,7,3,4,0,5,1};
L::byte rotateEdgesZ[] = {3,2,0,1,7,6,4,5,9,11,8,10};

template<class T, int n>
void permutate(T array[n], L::byte indices[]) {
  T tmp[n];
  for(int i(0); i<n; i++)
    tmp[i] = array[indices[i]];
  for(int i(0); i<n; i++)
    array[i] = tmp[i];
}
template<class T>
void reverse(T* array, int size) {
  for(int i(0); i<size/2; i++)
    swap(array[i],array[(size-i)-1]);
}
template<class T, int n>
void change(T array[n], T changes[]) {
  for(int i(0); i<n && array[i]!=0xFF; i++)
    array[i] = changes[array[i]];
}
L::byte getIndex(bool cell[8]) {
  L::byte wtr;
  for(int i(0); i<8; i++)
    if(cell[i])
      wtr |= (L::byte)(1 << i);
  return wtr;
}
L::byte getIndex(Voxel cell[8]) {
  L::byte wtr(0);
  for(int i(0); i<8; i++)
    if(cell[i].solid())
      wtr |= (L::byte)(1 << i);
  return wtr;
}
void reverseMesh(L::byte mesh[15]) {
  int end(0);
  while(end<15 && mesh[end]!=0xFF) end++;
  reverse<L::byte>(mesh,end);
}
void ChunkMesher::generateCase(bool cell[8],L::byte mesh[15], bool complementary) {
  for(int x(0); x<4; x++) {
    for(int y(0); y<4; y++) {
      for(int z(0); z<4; z++) {
        L::byte index(getIndex(cell));
        if(meshes[index][0]==meshes[index][1]) // Hasn't been set yet
          memcpy(meshes[index],mesh,15);
        permutate<bool,8>(cell,rotateCornersZ);
        change<L::byte,15>(mesh,rotateEdgesZ);
      }
      permutate<bool,8>(cell,rotateCornersY);
      change<L::byte,15>(mesh,rotateEdgesY);
    }
    permutate<bool,8>(cell,rotateCornersX);
    change<L::byte,15>(mesh,rotateEdgesX);
  }
  if(complementary) {
    for(int i(0); i<8; i++)
      cell[i] = !cell[i];
    reverseMesh(mesh);
    generateCase(cell,mesh,false);
  }
}
uint ChunkMesher::vertex(const Point3i& offset, int x, int y, int z, L::byte edge, Voxel cell[8]) {
  uint wtr;
  int indexEdge(edge);
  int indexX(x);
  int indexY(y);
  int indexZ(z);
  static const int size(Chunk::size);
  static const int sizePlus(size+1);
  switch(edge) { // Avoid doubles
    case backBottom:
      indexEdge = 0;
      break;
    case backLeft:
      indexEdge = 1;
      break;
    case bottomLeft:
      indexEdge = 2;
      break;
    case backTop:
      indexEdge = 0; //backBottom;
      indexY++;
      break;
    case backRight:
      indexEdge = 1; //backLeft;
      indexX++;
      break;
    case frontBottom:
      indexEdge = 0; //backBottom;
      indexZ++;
      break;
    case frontTop:
      indexEdge = 0; //backBottom;
      indexY++;
      indexZ++;
      break;
    case frontLeft:
      indexEdge = 1; //backLeft;
      indexZ++;
      break;
    case frontRight:
      indexEdge = 1; //backLeft;
      indexX++;
      indexZ++;
      break;
    case bottomRight:
      indexEdge = 2; //bottomLeft;
      indexX++;
      break;
    case topLeft:
      indexEdge = 2; //bottomLeft;
      indexY++;
      break;
    case topRight:
      indexEdge = 2; //bottomLeft;
      indexX++;
      indexY++;
      break;
  }
  int index(3*((indexX * sizePlus * sizePlus) + (indexY * sizePlus) + indexZ)+indexEdge);
  if(_edgeVertices[index]<0) {
    Point3f vertex((float)x,(float)y,(float)z);
    switch(edge) {
      case backBottom:
        vertex.x() += edgeValue(cell[bbl],cell[bbr]);
        break;
      case backTop:
        vertex.x() += edgeValue(cell[btl],cell[btr]);
        vertex.y() += 1;
        break;
      case backLeft:
        vertex.y() += edgeValue(cell[bbl],cell[btl]);
        break;
      case backRight:
        vertex.x() += 1;
        vertex.y() += edgeValue(cell[bbr],cell[btr]);
        break;
      case frontBottom:
        vertex.x() += edgeValue(cell[fbl],cell[fbr]);
        vertex.z() += 1;
        break;
      case frontTop:
        vertex.x() += edgeValue(cell[ftl],cell[ftr]);
        vertex.y() += 1;
        vertex.z() += 1;
        break;
      case frontLeft:
        vertex.y() += edgeValue(cell[fbl],cell[ftl]);
        vertex.z() += 1;
        break;
      case frontRight:
        vertex.x() += 1;
        vertex.y() += edgeValue(cell[fbr],cell[ftr]);
        vertex.z() += 1;
        break;
      case bottomLeft:
        vertex.z() += edgeValue(cell[bbl],cell[fbl]);
        break;
      case bottomRight:
        vertex.x() += 1;
        vertex.z() += edgeValue(cell[bbr],cell[fbr]);
        break;
      case topLeft:
        vertex.y() += 1;
        vertex.z() += edgeValue(cell[btl],cell[ftl]);
        break;
      case topRight:
        vertex.x() += 1;
        vertex.y() += 1;
        vertex.z() += edgeValue(cell[btr],cell[ftr]);
        break;
    }
    vertex += offset;
    _meshBuilder.setVertex(vertex);
    _meshBuilder.setVertexColor(color(edge,cell));
    wtr = _edgeVertices[index] = _meshBuilder.addVertex();
  } else wtr = _edgeVertices[index];
  return wtr;
}
Color ChunkMesher::color(L::byte edge, Voxel cell[8]) {
  switch(edge) {
#define TMP(e,c1,c2) case e: return (cell[c1].solid())?cell[c1].color():cell[c2].color();
      TMP(backBottom,bbl,bbr)
      TMP(backTop,btl,btr)
      TMP(backLeft,bbl,btl)
      TMP(backRight,bbr,btr)
      TMP(frontBottom,fbl,fbr)
      TMP(frontTop,ftl,ftr)
      TMP(frontLeft,fbl,ftl)
      TMP(frontRight,fbr,ftr)
      TMP(bottomLeft,bbl,fbl)
      TMP(bottomRight,bbr,fbr)
      TMP(topLeft,btl,ftl)
      TMP(topRight,btr,ftr)
#undef TMP
    default:
      return Color::white;
  }
}
float ChunkMesher::edgeValue(Voxel v1, Voxel v2) {
  return (v1.value()-.5f)/(v1.value()-v2.value());
}
#define LIST(...) {__VA_ARGS__}
#define GENERATE_CASE(n,cell,triangles,comp) \
  bool n##c[8] = LIST cell; \
  L::byte n##t[15] = LIST triangles; \
  generateCase(n##c,n##t,comp);
ChunkMesher::ChunkMesher() {
  static bool init(true);
  if(!init) {
    init = true;
    GENERATE_CASE(nothing,(false,false,false,false,false,false,false,false),
                  (0xFF),true);
    GENERATE_CASE(singleCorner,(true,false,false,false,false,false,false,false),
                  (backBottom,bottomLeft,backLeft,
                   0xFF),true);
    GENERATE_CASE(singleEdge,(true,true,false,false,false,false,false,false),
                  (backLeft,bottomRight,bottomLeft,
                   bottomRight,backLeft,backRight,
                   0xFF),true);
    GENERATE_CASE(shallowDoubleCorner,(true,false,false,true,false,false,false,false),
                  (backBottom,bottomLeft,backLeft,
                   backRight,backTop,topRight,
                   0xFF),false);
    GENERATE_CASE(plainDoubleCorner,(false,true,true,false,true,true,true,true),
                  (backTop,bottomLeft,topRight,
                   bottomLeft,backTop,backLeft,
                   backBottom,topRight,bottomLeft,
                   topRight,backBottom,backRight,
                   0xFF),false);
    GENERATE_CASE(triangle,(true,true,true,false,false,false,false,false),
                  (bottomRight,bottomLeft,topLeft,
                   bottomRight,backTop,backRight,
                   backTop,bottomRight,topLeft,
                   0xFF),true);
    GENERATE_CASE(quad,(true,true,true,true,false,false,false,false),
                  (bottomRight,topLeft,topRight,
                   topLeft,bottomRight,bottomLeft,
                   0xFF),true);
    GENERATE_CASE(shallowTriangleCorner,(false,true,true,false,true,true,false,false),
                  (backBottom,backRight,bottomLeft,
                   bottomLeft,backRight,frontLeft,
                   backRight,frontRight,frontLeft,
                   backLeft,topLeft,backTop,
                   0xFF),false);
    GENERATE_CASE(plainTriangleCorner,(true,false,false,true,false,false,true,true),
                  (backBottom,bottomLeft,backLeft,
                   backRight,backTop,frontLeft,
                   backTop,topLeft,frontLeft,
                   backRight,frontLeft,frontRight,
                   0xFF),false);
    GENERATE_CASE(shallowQuadrupleCorner,(true,false,false,true,false,true,true,false),
                  (backBottom,bottomLeft,backLeft,
                   backRight,backTop,topRight,
                   frontBottom,bottomRight,frontRight,
                   frontLeft,frontTop,topLeft,
                   0xFF),false);
    GENERATE_CASE(hexagone,(true,true,true,false,true,false,false,false),
                  (bottomRight,backTop,backRight,
                   bottomRight,topLeft,backTop,
                   topLeft,bottomRight,frontBottom,
                   frontBottom,frontLeft,topLeft,
                   0xFF),true);
    GENERATE_CASE(weird1,(true,false,false,false,true,true,false,true),
                  (backLeft,backBottom,frontLeft,
                   backBottom,frontTop,frontLeft,
                   frontTop,backBottom,topRight,
                   backBottom,bottomRight,topRight,
                   0xFF),true);
    GENERATE_CASE(opposingCorners,(true,false,false,false,false,false,false,true),
                  (backBottom,bottomLeft,backLeft,
                   frontRight,topRight,frontTop,
                   0xFF),true);
    GENERATE_CASE(shallowEdgeCorner,(true,true,false,false,false,false,false,true),
                  (backLeft,bottomRight,bottomLeft,
                   bottomRight,backLeft,backRight,
                   frontRight,topRight,frontTop,
                   0xFF),false);
    GENERATE_CASE(plainEdgeCorner,(false,false,true,true,true,true,true,false),
                  (topRight,backRight,frontTop,
                   backRight,backLeft,frontTop,
                   backLeft,bottomLeft,frontTop,
                   bottomLeft,bottomRight,frontTop,
                   bottomRight,frontRight,frontTop),false);
    GENERATE_CASE(shallowTripleCorner,(false,true,true,false,false,false,false,true),
                  (backBottom,bottomRight,backRight,
                   backLeft,backTop,topLeft,
                   frontRight,frontTop,topRight,
                   0xFF),false);
    GENERATE_CASE(plainTripleCorner,(true,false,false,true,true,true,true,false),
                  (backBottom,topLeft,backLeft,
                   backBottom,frontTop,topLeft,
                   backBottom,bottomRight,frontTop,
                   bottomRight,frontRight,frontTop,
                   backLeft,backTop,topRight),false);
    GENERATE_CASE(shallowOpposingEdges,(true,true,false,false,false,false,true,true),
                  (backLeft,bottomRight,bottomLeft,
                   bottomRight,backLeft,backRight,
                   frontLeft,topRight,topLeft,
                   topRight,frontLeft,frontRight,
                   0xFF),false);
    GENERATE_CASE(plainOpposingEdges,(false,false,true,true,true,true,false,false),
                  (backLeft,topLeft,topRight,
                   backLeft,topRight,backRight,
                   bottomLeft,frontLeft,frontRight,
                   bottomLeft,frontRight,bottomRight,
                   0xFF),false);
    GENERATE_CASE(weird2,(false,true,false,false,true,true,true,false),
                  (bottomLeft,backBottom,topLeft,
                   backBottom,frontRight,topLeft,
                   frontRight,frontTop,topLeft,
                   backBottom,backRight,frontRight,
                   0xFF),true);
    std::cout << '{';
    for(int i(0); i<256; i++) {
      std::cout << '{';
      for(int j(0); j<15; j++)
        std::cout << (int)meshes[i][j] << ((j<14)?',':'}');
      std::cout << ((i<255)?',':'}');
    }
  }
}

void ChunkMesher::build(Chunk& chunk) {
  memset(_edgeVertices,~0,sizeof(_edgeVertices));
  _meshBuilder.reset(GL::Mesh::VERTEX|GL::Mesh::COLOR|GL::Mesh::NORMAL,262144,262144);
  Point3i offset(chunk.position());
  for(int x(0); x<Chunk::size; x++)
    for(int y(0); y<Chunk::size; y++)
      for(int z(0); z<Chunk::size; z++) {
        Voxel cell[8] = {chunk.voxel(x,y,z),
                         chunk.voxel(x+1,y,z),
                         chunk.voxel(x,y+1,z),
                         chunk.voxel(x+1,y+1,z),
                         chunk.voxel(x,y,z+1),
                         chunk.voxel(x+1,y,z+1),
                         chunk.voxel(x,y+1,z+1),
                         chunk.voxel(x+1,y+1,z+1)
                        };
        const L::byte* mesh(meshes[getIndex(cell)]);
        if(mesh[0]!=mesh[1])
          for(int i(0); i<15 && mesh[i]!=0xFF; i+=3)
            _meshBuilder.addTriangle(vertex(offset,x,y,z,mesh[i],cell),
                                     vertex(offset,x,y,z,mesh[i+2],cell),
                                     vertex(offset,x,y,z,mesh[i+1],cell));
      }
  _meshBuilder.computeNormals();
}
