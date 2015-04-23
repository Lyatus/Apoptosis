#include <L/L.h>
#include <L/interface/stb.h>
#include <L/interface/obj.h>
#include <L/interface/wwise.h>
#include <L/interface/freetype.h>

#include "World.h"
#include "Automaton.h"
#include "shapes.h"
#include "SCA.h"

using namespace std;
using namespace L;

World world;
Window::Event event;
Ref<GUI::RelativeContainer> gui;
GL::Camera guicam;
bool tumorgrowing(false);

Voxel cancer(World& world, int x, int y, int z) {
  const Voxel& current(world.voxel(x,y,z));
  L::byte currentType(current.type());
  if(!tumorgrowing) return Voxel(current.value(),(currentType==Voxel::CANCER)?Voxel::CANCER_IDLE:currentType);
  if(currentType == Voxel::CANCER_IDLE) return current; // No modifications to idle cancer
  const Voxel& other(world.voxel(x+Rand::next(-1,2),y+Rand::next(-1,2),z+Rand::next(-1,2)));
  if(other.solid() && other.type()==Voxel::CANCER)
    if(currentType==Voxel::CANCER || (other.value()>.9 && other.type()==Voxel::CANCER
                                      && (currentType==Voxel::NOTHING || currentType==Voxel::CANCER_IDLE || current.value()<.6)))
      return Voxel(std::min(1.f,current.value()+Rand::next(.0f,64.f/1024.f)),Voxel::CANCER);
  return current; // No change
}
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
void fillTerrain(const Interval3i& interval) {
  Perlin<2> perlin(16);
  Point3i i(interval.min());
  float height(interval.max().y()-interval.min().y());
  while(i.increment(interval.min(),interval.max())) {
    float distance(i.y()-(((perlin.value(Point2f((float)(i.x()-interval.min().x())/32,(float)(i.z()-interval.min().z())/32))+1)/2)*height));
    float value(std::min(1.f,std::max(0.f,.5f-distance)));
    world.updateVoxel(i.x(),i.y(),i.z(),Voxel(value,Voxel::LUNG),Voxel::max);
  }
}
void cellPhase() {
}
void tumorPhase() {
  // Cameras initialization
  GL::Camera cam(Point3f(0,0,50));
  cam.perspective(60,Window::aspect(),.1f,512);
  guicam.pixels();
  // Light initialization
  GL::Light light;
  light.position(1,1,1,0);
  // Programs initialization
  GL::Program voxelProgram(GL::Shader(File("Shader/voxel.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("Shader/voxel.frag"),GL_FRAGMENT_SHADER));
  GL::Program debugProgram(GL::Shader(File("Shader/debug.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("Shader/debug.frag"),GL_FRAGMENT_SHADER));
  GL::Program guiProgram(GL::Shader(File("Shader/gui.vert"),GL_VERTEX_SHADER),
                         GL::Shader(File("Shader/gui.frag"),GL_FRAGMENT_SHADER));
  GL::Program ppProgram(GL::Shader(File("Shader/pp.vert"),GL_VERTEX_SHADER),
                        GL::Shader(File("Shader/pp.frag"),GL_FRAGMENT_SHADER));
  GL::PostProcess pp(Window::width(),Window::height());
  // Textures fetching
  Perlin<2> perlin(16);
  Image::Bitmap noise(256,256);
  for(int x(0); x<256; x++)
    for(int y(0); y<256; y++)
      noise(x,y) = Color(128+128*perlin.value(Point2f((float)(x-128)/32,(float)(y-128)/32)));
  //gui->place(new GUI::Image(noise),Point2i(10,10),GUI::TL,GUI::TL);
  GL::Texture voxelTexture(Image::Bitmap("Image/tissue.bmp"));
  GL::Texture voxelNormal(Image::Bitmap("Image/normal.bmp"));
  // World initialization
  Timer timer, atimer, scatimer, tumortimer;
  File file("world");
  if(false && file.exists())
    world.read(file.open("rb"));
  else {
    //world.fill(Curve(Point3f(0,-32,0),Point3f(32,0,0),Point3f(32,0,32),Point3f(0,32,32),64,.1),Voxel::LUNG,Voxel::max);
    //world.fill(Triangle(Point3f(0,4,0),Point3f(0,16,0),Point3f(16,0,0),1),Voxel::LUNG,Voxel::max);
    //fillObj("Model/intestine.obj",Voxel::LUNG);
    fillTerrain(Interval3i(Point3i(-128,0,-128),Point3i(128,8,128)));
    //world.fill(Curve(Point3f(-32,0,-32),Point3f(-32,16,-32),Point3f(32,16,32),Point3f(32,0,32),1,.01),Voxel::VESSEL,Voxel::max);
    world.write(file.open("wb"));
  }
  file.close();
  SCA sca(1,64);
  bool scaworking(false);
  sca.addBranch(SCA::Branch(NULL,Point3f(0,4,0),Point3f(0,-1,0)));
  Automaton automaton(world,cancer);
  int automatonTurns(0);
  bool automatonworking(false);
  cout << timer.since().fSeconds() << endl;
  Time start(Time::now());
  while(Window::loop()) {
    float deltaTime(timer.frame().fSeconds());
    int automatonUpdates(deltaTime*automaton.size()*240);
    world.update();
    Wwise::update();
    for(int i(-1); i<automatonUpdates; i++)
      automatonworking = automaton.update();
    if(automatonUpdates==0) automatonworking = false;
    if(tumortimer.since()>Time(0,0,5))
      tumorgrowing = false;
    //if(scatimer.every(Time(0,50)))
      scaworking = sca.update(world);
    while(Window::newEvent(event)) {
      if(gui->event(event)) continue;
      if(event.type == Window::Event::LBUTTONDOWN || event.type == Window::Event::RBUTTONDOWN) {
        Wwise::postEvent("click");
        Point3f hit;
        if(world.raycast(cam.position(),cam.screenToRay(Window::normalizedMousePosition()),hit,128)) {
          std::cout << automatonworking << " " << scaworking << std::endl;
          if(event.type == Window::Event::LBUTTONDOWN) {
            if(!automatonworking && !scaworking) {
              world.voxelSphere(hit,1,Voxel::CANCER,Voxel::max);
              automaton.include(hit);
              tumorgrowing = true;
              tumortimer.setoff();
            }
          } else {
            if(!automatonworking) sca.addTarget(hit);
          }
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
    pp.prerender();
    glMatrixMode(GL_MODELVIEW); // Rseset matrix
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
    world.draw(cam);
    debugProgram.use(); // Draw debug
    debugProgram.uniform("view",cam.view());
    debugProgram.uniform("projection",cam.projection());
    //GL::Utils::drawAxes();
    //automaton.drawDebug();
    //world.draw();
    pp.postrender(ppProgram);
    glClear(GL_DEPTH_BUFFER_BIT); // Start drawing GUI
    guiProgram.use();
    guiProgram.uniform("projection",guicam.projection());
    gui->draw(guiProgram);
    Window::swapBuffers();
  }
}
int main(int argc, char* argv[]) {
  // Interfaces initialization
  new STB();
  new OBJ();
  Font::set(new FTFont("Arial.ttf",128));
  // Window initialization
  Window::open("Cancer",16*100,9*100);
  // GUI initialization
  gui = new GUI::RelativeContainer(Point2i(Window::width(),Window::height()));
  guicam.pixels();
  //gui->place(new GUI::Image(Image::Bitmap("Image/chat.png")),Point2i(10,10),GUI::TL,GUI::TL);
  //gui->place(new GUI::Rectangle(Point2i(256,128),Color::blue),Point2i(-10,10),GUI::TR,GUI::TR);
  //gui->place(new GUI::TextInput(Point2i(1024,128)),Point2i(10,-10),GUI::CL,GUI::CL);
  // Wwise initialization
  Wwise::init(AKTEXT("Wwise/"));
  Wwise::registerObject(100);
  Wwise::loadBank(AKTEXT("Init.bnk"));
  Wwise::loadBank(AKTEXT("Main.bnk"));
  // OpenGL initialization
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(.8f,.8f,.8f,1.f);
  // Iterate through phases
  cellPhase();
  tumorPhase();
  // Terminate
  Wwise::term();
  return 0;
}
