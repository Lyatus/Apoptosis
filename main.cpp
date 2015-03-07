#include <L/L.h>
#include <L/interface/bmp.h>
//#include <L/interface/obj.h>

#include "World.h"
#include "Automaton.h"

using namespace std;
using namespace L;

World world;
Automaton automaton(world,Interval3i(Point3i(-16,-16,-16),Point3i(16,16,16)),Automaton::cancer);

int main(int argc, char* argv[]) {
  new BMP();
  Window::Event e;
  Window::open("Cancer",16*64,9*64);
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
  Timer timer, ui;
  world.voxelSphere(Point3f(0,0,0),16,Voxel::LUNG);
  Time start(Time::now());
  while(Window::opened()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    for(int i(0); i<8000; i++)
      automaton.update();
    while(Window::newEvent(e)) {
      if(e.type == Window::Event::KEYDOWN)
        if(e.key==Window::Event::SPACE) {
          Point3f hit;
          if(world.raycast(cam.position(),cam.forward(),hit,32))
            world.voxelSphere(hit,4,Voxel::CANCER,Voxel::add);
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
    voxelProgram.uniform("ambientLevel",.4f);
    voxelProgram.uniform("time",(Time::now()-start).fSeconds());
    voxelProgram.uniform("view",cam.view());
    voxelProgram.uniform("texture",voxelTexture,GL_TEXTURE0);
    voxelProgram.uniform("normal",voxelNormal,GL_TEXTURE1);
    voxelProgram.uniform("eye",cam.position());
    world.draw();
    debugProgram.use();
    debugProgram.uniform("view",cam.view());
    //automaton.drawDebug();
    Window::swapBuffers();
  }
  return 0;
}
