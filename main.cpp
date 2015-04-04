#include <L/L.h>
#include <L/interface/bmp.h>
//#include <L/interface/obj.h>
#include <L/interface/wwise.h>

#include "World.h"
#include "Automaton.h"
#include "shapes.h"

using namespace std;
using namespace L;

World world;
Automaton automaton(world,Automaton::cancer);

void fillObj(const char* filename) {
  Vector<Point3f> vertices;
  File file(filename);
  file.open("r");
  List<String> line;
  while((line = file.readLine().explode(' ')).size()>0) {
    if(line[0]=="v")
      vertices.push_back(Point3f(FromString<float>(line[1]),FromString<float>(line[2]),FromString<float>(line[3]))*.3);
    else if(line[0]=="f")
      world.fill(Triangle(vertices[FromString<int>(line[1])-1],vertices[FromString<int>(line[2])-1],vertices[FromString<int>(line[3])-1],1),Voxel::LUNG,Voxel::max);
  }
}
int main(int argc, char* argv[]) {
  new BMP();
  Window::Event e;
  Window::open("Cancer",16*100,9*100);
  Wwise wwise;
  wwise.registerObject(100);
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
  File file("world");
  if(file.exists())
    world.read(file.open("rb"));
  else {
    //world.fill(Curve(Point3f(0,-32,0),Point3f(128,0,0),Point3f(128,0,128),Point3f(0,32,32),1,.01),Voxel::VESSEL,Voxel::max);
    //world.fill(Curve(Point3f(0,-32,0),Point3f(32,0,0),Point3f(32,0,32),Point3f(0,32,32),64,.1),Voxel::LUNG,Voxel::max);
    //world.fill(Triangle(Point3f(0,4,0),Point3f(0,16,0),Point3f(16,0,0),1),Voxel::LUNG,Voxel::max);
    fillObj("intestine.obj");
    world.write(file.open("wb"));
  }
  cout << timer.since().fSeconds() << endl;
  Time start(Time::now());
  while(Window::loop()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    wwise.update();
    while(!atimer.every(Time(0,16)))
      automaton.update();
    while(Window::newEvent(e)) {
      if(e.type == Window::Event::LBUTTONDOWN) {
        wwise.postEvent("click");
        Point3f hit;
        if(world.raycast(cam.position(),cam.screenToRay(Window::normalizedMousePosition()),hit,64)) {
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
    if(Window::isPressed(Window::Event::Z))
      cam.theta(2*deltaTime);
    if(Window::isPressed(Window::Event::S))
      cam.theta(-2*deltaTime);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    light.set(GL_LIGHT0);
    voxelProgram.use();
    voxelProgram.uniform("ambientLevel",.1f);
    voxelProgram.uniform("time",(Time::now()-start).fSeconds());
    voxelProgram.uniform("view",cam.view());
    voxelProgram.uniform("projection",cam.projection());
    voxelProgram.uniform("texture",voxelTexture,GL_TEXTURE0);
    voxelProgram.uniform("normal",voxelNormal,GL_TEXTURE1);
    voxelProgram.uniform("eye",cam.position());
    world.draw();
    debugProgram.use();
    debugProgram.uniform("view",cam.view());
    debugProgram.uniform("projection",cam.projection());
    //GL::Utils::drawAxes();
    //automaton.drawDebug();
    //world.draw();
    Window::swapBuffers();
  }
  return 0;
}
