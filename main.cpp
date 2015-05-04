#include <L/L.h>
#include <L/interface/stb.h>
#include <L/interface/obj.h>
#include <L/interface/json.h>
#include <L/interface/wwise.h>
#include <L/interface/freetype.h>

#include "World.h"
#include "Automaton.h"
#include "shapes.h"
#include "SCA.h"
#include "Conf.h"
#include "SphericalCamera.h"

using namespace std;
using namespace L;

World world;
SCA sca(4,512);
Window::Event event;
Ref<GUI::RelativeContainer> gui;
GL::Camera guicam;
SphericalCamera cam(Point3f(0,0,128));
bool tumorgrowing(false);
float irrigationRadius, growthFactor;
int turnsPerSecond;

Voxel tumorGrowth(World& world, int x, int y, int z, bool& processable) {
  Voxel current(world.voxel(x,y,z));
  L::byte currentType(current.type());
  bool idle(currentType==Voxel::TUMOR_IDLE || currentType==Voxel::TUMOR_THIRSTY_IDLE);
  if(tumorgrowing) { // The tumor is still growing
    Voxel other(world.voxel(x+Rand::nextInt()%2,y+Rand::nextInt()%2,z+Rand::nextInt()%2));
    L::byte otherType(other.type());
    bool otherTumor(otherType==Voxel::TUMOR || otherType==Voxel::TUMOR_THIRSTY);
    bool currentTumor(currentType==Voxel::TUMOR || currentType==Voxel::TUMOR_THIRSTY);
    if(other.solid() // Other needs to exist
        && otherTumor // Other needs to be tumor
        && !current.full() // Current shouldn't already be filled (avoid unnecessary calculations)
        && (currentTumor || other.full())) {
      float distance(sca.distance(Point3f(x,y,z),irrigationRadius-.9f));
      current.value(std::min(1.f,current.value()+(Rand::nextFloat()*(irrigationRadius/distance)*growthFactor)));
      if(!currentTumor) {
        bool thirsty(distance>irrigationRadius-1);
        if(thirsty) {
          if(idle) current.type(Voxel::TUMOR_THIRSTY_IDLE);
          else current.type(Voxel::TUMOR_THIRSTY);
        } else {
          if(idle) current.type(Voxel::TUMOR_IDLE);
          else current.type(Voxel::TUMOR);
        }
      }
    }
  } else // We need to stop the growth
    switch(currentType) {
      case Voxel::TUMOR:
        current.type(Voxel::TUMOR_IDLE);
        break;
      case Voxel::TUMOR_THIRSTY:
        current.type(Voxel::TUMOR_THIRSTY_IDLE);
        break;
      default:
        current.type(currentType);
        break;
    }
  processable = current.type()==Voxel::TUMOR || current.type()==Voxel::TUMOR_THIRSTY;
  return current;
}
Voxel tumorThirst(World& world, int x, int y, int z, bool& processable) {
  Voxel wtr(world.voxel(x,y,z));
  if(!wtr.solid())
    wtr.type(Voxel::NOTHING);
  else if(wtr.type()==Voxel::TUMOR_THIRSTY_IDLE
          && !world.voxel(x+Rand::nextInt()%2,y+Rand::nextInt()%2,z+Rand::nextInt()%2).solid()) {
    float value(std::max(0.f,wtr.value()-Rand::nextFloat()*growthFactor));
    wtr.value(value);
    float distance(sca.distance(Point3f(x,y,z),irrigationRadius+.5f));
    if(value<=Shape::fromDistance(distance-irrigationRadius))
      wtr.type(Voxel::TUMOR_IDLE);
  }
  processable = wtr.type()==Voxel::TUMOR_THIRSTY_IDLE;
  return wtr;
}
Automaton tumorGrowthAutomaton(world,tumorGrowth), tumorThirstAutomaton(world,tumorThirst);

void foreachChunk(Chunk* chunk) {
  if(chunk->typeCount(Voxel::TUMOR_THIRSTY_IDLE))
    for(int x(0); x<Chunk::size; x++)
      for(int y(0); y<Chunk::size; y++)
        for(int z(0); z<Chunk::size; z++)
          if(chunk->voxel(x,y,z).type()==Voxel::TUMOR_THIRSTY_IDLE) {
            Point3i position(chunk->position()+Point3i(x,y,z));
            if(sca.distance(position,irrigationRadius-.9f)>irrigationRadius-1)
              tumorThirstAutomaton.include(position);
            else world.updateVoxel(position.x(),position.y(),position.z(),Voxel(chunk->voxel(x,y,z).value(),Voxel::TUMOR_IDLE),Voxel::set);
          }
  if(chunk->typeCount(Voxel::TUMOR))
    cam.addPoint(chunk->position()+Point3i(Chunk::size/2,Chunk::size/2,Chunk::size/2));
}
void fillObj(const char* filename, byte type) {
  Vector<Point3f> vertices;
  File file(filename);
  file.open("r");
  List<String> line;
  while((line = file.readLine().explode(' ')).size()>0) {
    if(line[0]=="v")
      vertices.push_back(Point3f(FromString<float>(line[1]),FromString<float>(line[2]),FromString<float>(line[3])));
    else if(line[0]=="f")
      world.fill(Triangle(vertices[FromString<int>(line[1])-1],vertices[FromString<int>(line[2])-1],vertices[FromString<int>(line[3])-1],1),type,Voxel::max);
  }
}
void fillTerrain(const Interval3i& interval) {
  Perlin<2> perlin(16);
  Point3i i(interval.min());
  float height(interval.max().y()-interval.min().y());
  while(i.increment(interval.min(),interval.max())) {
    float distance(i.y()-(((perlin.value(Point2f((float)(i.x()-interval.min().x())/32,(float)(i.z()-interval.min().z())/32))+1)/2)*height));
    float value(Shape::fromDistance(distance));
    world.updateVoxel(i.x(),i.y(),i.z(),Voxel(value,Voxel::ORGAN),Voxel::max);
  }
}
void tumorPhase() {
  // Cameras initialization
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
  GL::Texture voxelTexture(Image::Bitmap("Image/tissue.bmp"));
  GL::Texture voxelNormal(Image::Bitmap("Image/normal.bmp"));
  // World initialization
  Timer timer, tumortimer, thirsttimer;
  File file("world");
  if(file.exists())
    world.read(file.open("rb"));
  else {
    fillObj("Model/intestine.obj",Voxel::ORGAN);
    world.write(file.open("wb"));
  }
  file.close();
  bool scaworking(false);
  sca.addBranch(SCA::Branch(NULL,Point3f(0,0,0),Point3f(0,0,0)));
  int automatonTurns(0);
  bool automatonWorking(false);
  cout << timer.since().fSeconds() << endl;
  Time start(Time::now());
  while(Window::loop()) {
    float deltaTime(timer.frame().fSeconds());
    world.update();
    Wwise::update();
    cam.update(deltaTime);
    tumorGrowthAutomaton.update((int)(deltaTime*turnsPerSecond));
    tumorThirstAutomaton.update((int)(deltaTime*turnsPerSecond));
    if(tumortimer.since()>Time(0,0,5))
      tumorgrowing = false;
    if(thirsttimer.every(Time(0,100)))
      world.foreachChunk(foreachChunk);
    scaworking = sca.update(world);
    while(Window::newEvent(event)) {
      if(gui->event(event)) continue;
      if(event.type == Window::Event::LBUTTONDOWN || event.type == Window::Event::RBUTTONDOWN) {
        Wwise::postEvent("click");
        Point3f hit;
        if(world.raycast(cam.position(),cam.screenToRay(Window::normalizedMousePosition()),hit,512)) {
          if(event.type == Window::Event::LBUTTONDOWN) {
            if(!tumorgrowing && !scaworking) {
              world.voxelSphere(hit,1,Voxel::TUMOR,Voxel::max);
              tumorGrowthAutomaton.include(hit);
              tumorgrowing = true;
              tumortimer.setoff();
            }
          } else {
            if(!tumorgrowing) sca.addTarget(hit);
          }
        }
      }
      cam.event(event);
    }
    if(Window::isPressed(Window::Event::ESCAPE))
      break;
    /*
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
    */
    pp.prerender();
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
    world.draw(cam);
    debugProgram.use(); // Draw debug
    debugProgram.uniform("view",cam.view());
    debugProgram.uniform("projection",cam.projection());
    //GL::Utils::drawAxes();
    //tumorGrowthAutomaton.drawDebug();
    //tumorThirstAutomaton.drawDebug();
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
  new JSON();
  Font::set(new FTFont("Arial.ttf",128));
  // Configuration fetching
  Conf::open("conf.json");
  Voxel::configure();
  SCA::configure();
  Color background(Conf::getColor("background"));
  irrigationRadius = Conf::getFloat("irrigation_radius");
  growthFactor = Conf::getFloat("growth_factor");
  turnsPerSecond = Conf::getInt("turns_per_second");
  // Window initialization
  if(Conf::getBool("fullscreen"))
    Window::openFullscreen("Apoptosis");
  else Window::open("Apoptosis",Conf::getInt("width"),Conf::getInt("height"));
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
  glClearColor((float)background.r()/255,(float)background.g()/255,(float)background.b()/255,1.f);
  // Iterate through phases
  tumorPhase();
  // Terminate
  Wwise::term();
  return 0;
}
