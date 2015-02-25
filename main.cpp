#include <L/L.h>

#include "World.h"

using namespace std;
using namespace L;

World world;

int main(int argc, char* argv[]) {
  Window::Event e;
  Window::open("Test",16*64,9*64);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  GL::Camera cam(Point3f(0,0,50));
  GL::Program program(GL::Shader(File("vert.glsl"),GL_VERTEX_SHADER),GL::Shader(File("frag.glsl"),GL_FRAGMENT_SHADER));
  GL::Light light;
  light.setPosition(5,5,5,0);
  Timer timer, ui;
  world.voxelSphere(Point3f(0,0,0),30);
  Time start(Time::now());
  while(Window::opened()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    while(Window::newEvent(e)) {}
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
    if(Window::isPressed(Window::Event::SPACE)) {
      Point3f hit;
      if(world.raycast(cam.position(),cam.forward(),hit,32))
        world.voxelSphere(hit,4,Voxel::sub);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam.update();
    cam.place();
    light.set(GL_LIGHT0);
    GL::Program::unuse();
    GL::Utils::drawAxes();
    program.use();
    program.uniform("ambientLevel",.4f);
    program.uniform("time",(Time::now()-start).fSeconds());
    program.uniform("view",cam.view());
    glPushMatrix();
    world.draw();
    glPopMatrix();
    Window::swapBuffers();
  }
  return 0;
}
