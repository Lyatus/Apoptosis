#include "ChunkMesher.h"

#include "Chunk.h"

using namespace L;
using namespace GL;

L::byte ChunkMesher::meshes[256][5*3] = {{0}};

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
template<class T, int n>
void reverse(T* array, int size) {
  T tmp[n];
  for(int i(0); i<size; i++)
    tmp[i] = array[(size-i)-1];
  for(int i(0); i<size; i++)
    array[i] = tmp[i];
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
  while(mesh[end]!=0xFF) end++;
  reverse<L::byte,15>(mesh,end);
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
uint ChunkMesher::vertex(int x, int y, int z, L::byte edge, Voxel cell[8]) {
  uint wtr;
  int indexEdge = edge;
  int indexX = x;
  int indexY = y;
  int indexZ = z;
  int size = Chunk::size;
  int sizePlus = size+1;
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
  int index = 3*((indexX * sizePlus * sizePlus) + (indexY * sizePlus) + indexZ)+indexEdge;
  if(_edgeVertices[index]<0) {
    Point3f vertex((float)x,(float)y,(float)z);
    color(edge,cell);
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
    _meshBuilder.setVertex(vertex);
    wtr = _meshBuilder.addVertex();
    _edgeVertices[index] = wtr;
  } else wtr = _edgeVertices[index];
  return wtr;
}
void ChunkMesher::color(L::byte edge, Voxel cell[8]) {
  Color c;
  switch(edge) {
      //#define TMP(e,c1,c2) case e: c = Color::lerp(cell[c1].color(),cell[c2].color(),1.f-edgeValue(cell[c1],cell[c2])); break;
#define TMP(e,c1,c2) case e: c = (cell[c1].solid())?cell[c1].color():cell[c2].color(); break;
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
      c = Color::white;
      break;
  }
  _meshBuilder.setVertexColor(c);
}
float ChunkMesher::edgeValue(Voxel v1, Voxel v2) {
  return (v1.value()-.5f)/(v1.value()-v2.value());
}
#define LIST(...) {__VA_ARGS__}
#define GENERATE_CASE(n,cell,triangles,comp) \
  bool n##c[] = LIST cell; \
  L::byte n##t[] = LIST triangles; \
  generateCase(n##c,n##t,comp);
ChunkMesher::ChunkMesher() {
  static bool init(false);
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
                   bottomRight,frontRight,frontTop,
                   0xFF),false);
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
                   backLeft,backTop,topRight,
                   0xFF),false);
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
  }
}

void ChunkMesher::build(Chunk& chunk) {
  memset(_edgeVertices,~0,sizeof(_edgeVertices));
  _meshBuilder.reset(GL::Mesh::VERTEX|GL::Mesh::COLOR|GL::Mesh::NORMAL,65536,65536);
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
        L::byte* mesh = meshes[getIndex(cell)];
        if(mesh[0]!=mesh[1])
          for(int i(0); mesh[i]!=0xFF && i<15; i+=3)
            _meshBuilder.addTriangle(vertex(x,y,z,mesh[i],cell),
                                     vertex(x,y,z,mesh[i+2],cell),
                                     vertex(x,y,z,mesh[i+1],cell));
      }
  _meshBuilder.computeNormals();
}
