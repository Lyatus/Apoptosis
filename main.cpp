#include <L/L.h>
#include <L/interface/bmp.h>
//#include <L/interface/obj.h>

#include "World.h"
#include "Automaton.h"
#include "shapes.h"

using namespace std;
using namespace L;

World world;
Automaton automaton(world,Automaton::cancer);

int main(int argc, char* argv[]) {
  /*
  Matrix33f m;
  m(0,0) = 1;
  m(0,1) = 2;
  m(0,2) = 3;
  m(1,0) = 0;
  m(1,1) = 4;
  m(1,2) = 5;
  m(2,0) = 1;
  m(2,1) = 0;
  m(2,2) = 6;
  cout << m << endl;
  cout << m.inverse() << endl;
  return 0;
  */
  new BMP();
  Window::Event e;
  Window::open("Cancer",16*100,9*100);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(.8,.8,.8,1);
  GL::Camera cam(Point3f(0,0,50));
  GL::Program voxelProgram(GL::Shader(File("voxel.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("voxel.frag"),GL_FRAGMENT_SHADER));
  GL::Program debugProgram(GL::Shader(File("debug.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("debug.frag"),GL_FRAGMENT_SHADER));
  GL::Texture voxelTexture(Image::Bitmap("tissue.bmp"));
  GL::Texture voxelNormal(Image::Bitmap("normal.bmp"));
  GL::Light light;
  light.position(1,1,1,0);
  Timer timer, atimer;
  //world.fill(Curve(Point3f(0,-32,0),Point3f(128,0,0),Point3f(128,0,128),Point3f(0,32,32),1,.01),Voxel::VESSEL,Voxel::max);
  //world.fill(Curve(Point3f(0,-32,0),Point3f(32,0,0),Point3f(32,0,32),Point3f(0,32,32),64,.1),Voxel::LUNG,Voxel::max);
  //world.fill(Triangle(Point3f(0,4,0),Point3f(0,16,0),Point3f(16,0,0),1),Voxel::LUNG,Voxel::max);
  Vector<Point3f> vertices;
  File file("intestine.obj");
  file.open("r");
  List<String> line;
  while((line = file.readLine().explode(' ')).size()>0) {
    if(line[0]=="v")
      vertices.push_back(Point3f(FromString<float>(line[1]),FromString<float>(line[2]),FromString<float>(line[3]))*.2);
    else if(line[0]=="f")
      world.fill(Triangle(vertices[FromString<int>(line[1])-1],vertices[FromString<int>(line[2])-1],vertices[FromString<int>(line[3])-1],1),Voxel::LUNG,Voxel::max);
  }
  cout << timer.since().fSeconds() << endl;
  Time start(Time::now());
  while(Window::opened()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    while(!atimer.every(Time(0,16)))
      automaton.update();
    while(Window::newEvent(e)) {
      if(e.type == Window::Event::KEYDOWN)
        if(e.key == Window::Event::SPACE) {
          Point3f hit;
          if(world.raycast(cam.position(),cam.forward(),hit,32)) {
            world.voxelSphere(hit,1,Voxel::CANCER,Voxel::max);
            automaton.include(hit);
          }
        }
    }
    if(Window::isPressed(Window::Event::ESCAPE))
      break;
    if(Window::isPressed(Window::Event::SHIFT))
      cam.move(Point3f(0,24*deltaTime,0));
    if(Window::isPressed(Window::Event::CTRL))
      cam.move(Point3f(0,-24*deltaTime,0));
    if(Window::isPressed(Window::Event::UP))
      cam.move(Point3f(0,0,24*deltaTime));
    if(Window::isPressed(Window::Event::DOWN))
      cam.move(Point3f(0,0,-24*deltaTime));
    if(Window::isPressed(Window::Event::LEFT))
      cam.phi(2*deltaTime);
    if(Window::isPressed(Window::Event::RIGHT))
      cam.phi(-2*deltaTime);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam.update();
    cam.place();
    light.set(GL_LIGHT0);
    GL::Program::unuse();
    GL::Utils::drawAxes();
    voxelProgram.use();
    voxelProgram.uniform("ambientLevel",.1f);
    voxelProgram.uniform("time",(Time::now()-start).fSeconds());
    voxelProgram.uniform("view",cam.view());
    voxelProgram.uniform("texture",voxelTexture,GL_TEXTURE0);
    voxelProgram.uniform("normal",voxelNormal,GL_TEXTURE1);
    voxelProgram.uniform("eye",cam.position());
    world.draw();
    debugProgram.use();
    debugProgram.uniform("view",cam.view());
    //automaton.drawDebug();
    //world.draw();
    Window::swapBuffers();
  }
  return 0;
}
