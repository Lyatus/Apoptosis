#include <L/L.h>
#include <L/interface/stb.h>
#include <L/interface/obj.h>
#include <L/interface/wwise.h>
#include <L/interface/freetype.h>

#include "World.h"
#include "Automaton.h"
#include "shapes.h"

using namespace std;
using namespace L;

World world;
Automaton automaton(world,Automaton::cancer);

void fillObj(const char* filename, byte type) {
  Vector<Point3f> vertices;
  File file(filename);
  file.open("r");
  List<String> line;
  while((line = file.readLine().explode(' ')).size()>0)
    if(line[0]=="v")
      vertices.push_back(Point3f(FromString<float>(line[1]),FromString<float>(line[2]),FromString<float>(line[3]))*.3f);
    else if(line[0]=="f")
      world.fill(Triangle(vertices[FromString<int>(line[1])-1],vertices[FromString<int>(line[2])-1],vertices[FromString<int>(line[3])-1],1),type,Voxel::max);
}
int main(int argc, char* argv[]) {
  // Interfaces initialization
  new STB();
  new OBJ();
  Font::set(new FTFont("Arial.ttf",128));
  // Window initialization
  Window::Event e;
  Window::open("Cancer",16*100,9*100);
  // GUI initialization
  Ref<GUI::RelativeContainer> gui(new GUI::RelativeContainer(Point2i(Window::width(),Window::height())));
  //gui->place(new GUI::Image(Image::Bitmap("chat.png")),Point2i(10,10),GUI::TL,GUI::TL);
  //gui->place(new GUI::Rectangle(Point2i(256,128),Color::blue),Point2i(-10,10),GUI::TR,GUI::TR);
  //gui->place(new GUI::TextInput(Point2i(1024,128)),Point2i(10,-10),GUI::CL,GUI::CL);
  // Wwise initialization
  Wwise wwise(AKTEXT("Wwise/"));
  wwise.registerObject(100);
  wwise.loadBank(AKTEXT("Init.bnk"));
  wwise.loadBank(AKTEXT("Main.bnk"));
  // OpenGL initialization
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(.8f,.8f,.8f,1.f);
  // Cameras initialization
  GL::Camera cam(Point3f(0,0,50)), guicam(Point3f(0,0,0));
  cam.perspective(60,Window::aspect(),.1,512);
  guicam.pixels();
  // Light initialization
  GL::Light light;
  light.position(1,1,1,0);
  // Programs initialization
  GL::Program voxelProgram(GL::Shader(File("voxel.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("voxel.frag"),GL_FRAGMENT_SHADER));
  GL::Program debugProgram(GL::Shader(File("debug.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("debug.frag"),GL_FRAGMENT_SHADER));
  GL::Program guiProgram(GL::Shader(File("gui.vert"),GL_VERTEX_SHADER),
                         GL::Shader(File("gui.frag"),GL_FRAGMENT_SHADER));
  // Textures fetching
  GL::Texture voxelTexture(Image::Bitmap("tissue.bmp"));
  GL::Texture voxelNormal(Image::Bitmap("normal.bmp"));
  // World initialization
  Timer timer, atimer;
  File file("world");
  if(file.exists())
    world.read(file.open("rb"));
  else {
    //world.fill(Curve(Point3f(0,-32,0),Point3f(128,0,0),Point3f(128,0,128),Point3f(0,32,32),1,.01),Voxel::VESSEL,Voxel::max);
    //world.fill(Curve(Point3f(0,-32,0),Point3f(32,0,0),Point3f(32,0,32),Point3f(0,32,32),64,.1),Voxel::LUNG,Voxel::max);
    //world.fill(Triangle(Point3f(0,4,0),Point3f(0,16,0),Point3f(16,0,0),1),Voxel::LUNG,Voxel::max);
    fillObj("intestine.obj",Voxel::LUNG);
    world.write(file.open("wb"));
  }
  file.close();
  cout << timer.since().fSeconds() << endl;
  Time start(Time::now());
  while(Window::loop()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    wwise.update();
    while(!atimer.every(Time(0,16)))
      automaton.update();
    while(Window::newEvent(e)) {
      if(gui->event(e)) continue;
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
    glMatrixMode(GL_MODELVIEW); // Reset matrix
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Start drawing 3D
    light.set(GL_LIGHT0);
    voxelProgram.use(); // Draw voxels
    voxelProgram.uniform("ambientLevel",.1f);
    voxelProgram.uniform("time",(Time::now()-start).fSeconds());
    voxelProgram.uniform("view",cam.view());
    voxelProgram.uniform("projection",cam.projection());
    voxelProgram.uniform("texture",voxelTexture,GL_TEXTURE0);
    voxelProgram.uniform("normal",voxelNormal,GL_TEXTURE1);
    voxelProgram.uniform("eye",cam.position());
    world.draw();
    debugProgram.use(); // Draw debug
    debugProgram.uniform("view",cam.view());
    debugProgram.uniform("projection",cam.projection());
    //GL::Utils::drawAxes();
    //automaton.drawDebug();
    //world.draw();
    glClear(GL_DEPTH_BUFFER_BIT); // Start drawing GUI
    guiProgram.use();
    guiProgram.uniform("projection",guicam.projection());
    gui->draw(guiProgram);
    Window::swapBuffers();
  }
  return 0;
}
