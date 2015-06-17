#include "main.h" // I know it's wrong
#include <L/interface/stb.h>
#include <L/interface/obj.h>
#include <L/interface/json.h>
#include <L/interface/wwise.h>
#include <L/interface/freetype.h>

#include "Automaton.h"
#include "Bonus.h"
#include "Conf.h"
#include "Event.h"
#include "Game.h"
#include "Resource.h"
#include "SCA.h"
#include "shapes.h"
#include "SphericalCamera.h"
#include "UI.h"
#include "World.h"

using namespace std;
using namespace L;

World world;
SCA sca(2,2048);
Window::Event event;
Ref<GUI::RelativeContainer> gui;
GL::Camera guicam;
SphericalCamera cam;
Perlin<1> perlin(16);

// Graphic configuration
float ambientLevel;
bool displayAutomata(false), displayVessels(true), displayTargets(false), debugText(false);
float automataTPF, searchTPF;
// GUI configuration
float menuFadeDuration, gameFadeDuration, introDarkDuration, gameDuration;
Timer fadeTimer;

// Gameplay configuration
float resourceSpeed, resourceSpeedIdle, tumorCost, vesselCost, autoaimRadius;

float irrigationRadius;
Point3f irrigationSphereCenter;

float growthVPS, growthDuration, growthStartRadius, growthCount;
float thirstVPS, thirstAppearanceFactor;
float chemoVPS, chemoPropagationFactor, chemoOrganFactor, chemoDuration, chemoDisappearanceFactor, chemoCount;
float buddingVPS, buddingDuration, buddingMinY, buddingCount, buddingMaxDistance;
float vesselCount, burstRadius;
float buddingFactor, vesselBuddingFactor, chemoBuddingFactor;
float chemoBuddingCurve;
Time clickLapse;

// Gameplay tracking
float resource(1);
int tumorCount, tumorThirstyCount;

// Tutorial
int tutoStep(0);
Time tutoDelay, tutoChemoStart, tutoChemoEnd;
bool movedCamera(false), placedTumor(false);
Point3f tutoCameraPosition, tutoTumorPosition;

// Wwise plugin
AK_FUNC(AK::IAkPlugin *, CreateHV_Plug1_WwisePluginEngine)(AK::IAkPluginMemAlloc *in_pAllocator);
AK_FUNC(AK::IAkPluginParam *, CreateHV_Plug1_WwisePluginEngineParams)(AK::IAkPluginMemAlloc * in_pAllocator);

float irrigationValue(const Point3f& p) {
  return Shape::fromDistance(sca.distance(p,irrigationRadius+1)-irrigationRadius);
}
Voxel growth(Automaton& automaton, int x, int y, int z, bool& processable) {
  Voxel current(automaton.voxel(x,y,z));
  L::byte currentType(current.type());
  bool idle(currentType==Voxel::TUMOR_IDLE || currentType==Voxel::TUMOR_THIRSTY_IDLE);
  if(!automaton.shouldStop()) { // The tumor is still growing
    Voxel other(automaton.voxel(x+Rand::nextInt()%2,y+Rand::nextInt()%2,z+Rand::nextInt()%2));
    L::byte otherType(other.type());
    bool otherTumor(otherType==Voxel::TUMOR || otherType==Voxel::TUMOR_THIRSTY);
    bool currentTumor(currentType==Voxel::TUMOR || currentType==Voxel::TUMOR_THIRSTY);
    if(currentType==Voxel::TUMOR_START
        || (other.solid() // Other needs to exist
            && otherTumor // Other needs to be tumor
            && !current.full() // Current shouldn't already be filled (avoid unnecessary calculations)
            && (currentTumor || other.full()))) {
      current.value(std::min(1.f,current.value()+(Rand::nextFloat()*automaton.factor())));
      if(!currentTumor) {
        float value(irrigationValue(Point3f(x,y,z)));
        bool thirsty(value<1.f);
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
  processable = current.type()==Voxel::TUMOR_START || current.type()==Voxel::TUMOR || current.type()==Voxel::TUMOR_THIRSTY;
  return current;
}
Voxel thirst(Automaton& automaton, int x, int y, int z, bool& processable) {
  Voxel wtr(automaton.voxel(x,y,z));
  if(wtr.empty())
    wtr.type(Voxel::NOTHING);
  else if(wtr.type()==Voxel::TUMOR_THIRSTY_IDLE
          && (Rand::nextFloat()<.05f || automaton.voxel(x+Rand::nextInt()%2,y+Rand::nextInt()%2,z+Rand::nextInt()%2).empty())) {
    if(wtr.value()<=irrigationValue(Point3f(x,y,z)))
      wtr.type(Voxel::TUMOR_IDLE);
    else
      wtr.value(std::max(0.f,wtr.value()-Rand::nextFloat()*automaton.factor()));
  }
  processable = wtr.type()==Voxel::TUMOR_THIRSTY_IDLE;
  return wtr;
}
Voxel chemo(Automaton& automaton, int x, int y, int z, bool& processable) {
  Voxel wtr(automaton.voxel(x,y,z));
  if(automaton.shouldStop()) {
    if(Rand::nextFloat()<chemoDisappearanceFactor)
      switch(wtr.type()) {
        case Voxel::ORGAN_CHEMO:
          wtr.type(Voxel::ORGAN);
          break;
        case Voxel::TUMOR_CHEMO:
          wtr.type(Voxel::TUMOR_IDLE);
          break;
        case Voxel::TUMOR_THIRSTY_CHEMO:
          wtr.type(Voxel::TUMOR_THIRSTY_IDLE);
          break;
      }
  } else if(wtr.empty())
    wtr.type(Voxel::NOTHING);
  else {
    bool chemo(wtr.type()==Voxel::ORGAN_CHEMO || wtr.type()==Voxel::TUMOR_CHEMO || wtr.type()==Voxel::TUMOR_THIRSTY_CHEMO);
    Voxel other(automaton.voxel(x+Rand::nextInt()%2,y+Rand::nextInt()%2,z+Rand::nextInt()%2));
    bool otherChemo(other.type()==Voxel::ORGAN_CHEMO || other.type()==Voxel::TUMOR_CHEMO || other.type()==Voxel::TUMOR_THIRSTY_CHEMO);
    if(chemo && other.empty())
      wtr.value(std::max(0.f,wtr.value()-Rand::nextFloat()*automaton.factor()*((wtr.type()==Voxel::ORGAN_CHEMO)?chemoOrganFactor:1)));
    if(!chemo && otherChemo &&  Rand::nextFloat()<chemoPropagationFactor*((wtr.type()==Voxel::ORGAN)?chemoOrganFactor:1))
      switch(wtr.type()) {
        case Voxel::ORGAN:
          wtr.type(Voxel::ORGAN_CHEMO);
          break;
        case Voxel::TUMOR_IDLE:
          wtr.type(Voxel::TUMOR_CHEMO);
          break;
        case Voxel::TUMOR_THIRSTY_IDLE:
          wtr.type(Voxel::TUMOR_THIRSTY_CHEMO);
          break;
      }
  }
  processable = wtr.type()==Voxel::ORGAN_CHEMO || wtr.type()==Voxel::TUMOR_CHEMO || wtr.type()==Voxel::TUMOR_THIRSTY_CHEMO;
  return wtr;
}
void startTumor(const Point3f& start, float vps, const Time& duration) {
  Automaton* automaton(new Automaton(world,growth,vps,Game::now()+duration));
  world.voxelSphere(start,growthStartRadius,Voxel::TUMOR_START,Voxel::max);
  Automaton::add(automaton,"Tumor_start",start);
}
void startGrowth(const Point3f& start) {
  startTumor(start,growthVPS,Time(growthDuration*1000000.f));
}
void startBud(const Point3f& start) {
  startTumor(start,buddingVPS,Time(buddingDuration*1000000.f));
}
void startThirst(const Point3f& start) {
  Automaton* current(Automaton::get(thirst,start));
  if(current)
    current->include(start);
  else {
    Automaton* automaton(new Automaton(world,thirst,thirstVPS));
    Automaton::add(automaton,"Thirst_start",start);
  }
}
void startChemo(const Point3f& start) {
  Automaton* automaton(new Automaton(world,chemo,chemoVPS,Game::now()+Time(chemoDuration*1000000.f)));
  world.voxelSphere(start,1.f,Voxel::TUMOR_CHEMO,Voxel::max);
  Automaton::add(automaton,"Chemo_start",start);
}
void search() {
  Point3i ci(0), cmin(0), cmax(0);
  while(true) {
    if(!ci.increment(cmin,cmax)) {
      ci = cmin = world.interval().min()-World::radius;
      cmax = world.interval().max()-World::radius;
    }
    if(world.chunkExists(ci.x(),ci.y(),ci.z())) {
      Chunk* chunk(&world.chunk(ci.x(),ci.y(),ci.z()));
      bool thirstPotential(chunk->typeCount(Voxel::TUMOR_THIRSTY_IDLE));
      bool camPotential(chunk->typeCount(Voxel::TUMOR) || chunk->typeCount(Voxel::TUMOR_IDLE) || chunk->typeCount(Voxel::TUMOR_THIRSTY) || chunk->typeCount(Voxel::TUMOR_THIRSTY_IDLE));
      bool budPotential(chunk->typeCount(Voxel::TUMOR_IDLE) && chunk->typeCount(Voxel::ORGAN));
      bool vesselBudPotential(vesselBuddingFactor>0 && (chunk->typeCount(Voxel::TUMOR_THIRSTY) || chunk->typeCount(Voxel::TUMOR_THIRSTY_IDLE)));
      bool chemoBudPotential(chemoBuddingFactor>0 && chunk->typeCount(Voxel::TUMOR_IDLE));
      if(thirstPotential || camPotential || budPotential || vesselBudPotential || chemoBudPotential)
        for(int x(0); x<Chunk::size; x++)
          for(int y(0); y<Chunk::size; y++)
            for(int z(0); z<Chunk::size; z++) {
              Voxel voxel(chunk->voxel(x,y,z));
              Point3i position(chunk->position()+Point3i(x,y,z));
              if(thirstPotential && voxel.type()==Voxel::TUMOR_THIRSTY_IDLE && Rand::nextFloat()<thirstAppearanceFactor  && !Automaton::has(thirst,position)) {
                if(irrigationValue(Point3f(x,y,z))<1.f)
                  startThirst(position);
                else world.updateVoxel(position.x(),position.y(),position.z(),Voxel(chunk->voxel(x,y,z).value(),Voxel::TUMOR_IDLE),Voxel::set);
              }
              if(camPotential && (voxel.type()==Voxel::TUMOR || voxel.type()==Voxel::TUMOR_IDLE || voxel.type()==Voxel::TUMOR_THIRSTY || voxel.type()==Voxel::TUMOR_THIRSTY_IDLE))
                cam.addPoint(chunk->position()+Point3i(x,y,z));
              if(budPotential && voxel.type()==Voxel::TUMOR_IDLE && position.y()>buddingMinY && !Automaton::has(growth,(int)buddingCount) && Rand::nextFloat()<(buddingFactor * ((buddingMaxDistance-Bonus::distanceToInactive(position))/buddingMaxDistance)) && !Automaton::has(growth,position))
                startBud(position);
              if(vesselBudPotential && (voxel.type()==Voxel::TUMOR_THIRSTY || voxel.type()==Voxel::TUMOR_THIRSTY_IDLE) && Rand::nextFloat()<vesselBuddingFactor)
                sca.addTarget(position);
              if(chemoBudPotential && voxel.type()==Voxel::TUMOR_IDLE && !Automaton::has(chemo,(int)chemoCount) && Rand::nextFloat()<(chemoBuddingFactor/tumorCount)*(chemoBuddingCurve/(Bonus::distanceToActive(position)+chemoBuddingCurve)) && !Automaton::has(chemo,position))
                startChemo(position);
            }
      Coroutine::yield();
    }
  }
}
void fillObj(const char* filename, byte type) {
  Array<Point3f> vertices;
  File file(filename);
  file.open("r");
  List<String> line;
  while((line = file.readLine().explode(' ')).size()>0) {
    if(line[0]=="v")
      vertices.push(Point3f(FromString<float>(line[1]),FromString<float>(line[2]),FromString<float>(line[3])));
    else if(line[0]=="f")
      world.fill(Triangle(vertices[FromString<int>(line[1])-1],vertices[FromString<int>(line[2])-1],vertices[FromString<int>(line[3])-1],4),type,Voxel::max);
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
void mask(const Color& color) {
  glClear(GL_DEPTH_BUFFER_BIT);
  GL::Program::unuse();
  GL::color(color);
  glBegin(GL_QUADS);
  glVertex2f(-1,-1);
  glVertex2f(1,-1);
  glVertex2f(1,1);
  glVertex2f(-1,1);
  glEnd();
}
void clearcolor(const Color& color) {
  glClearColor((float)color.r()/255,(float)color.g()/255,(float)color.b()/255,(float)color.a()/255);
}
void menu() {
  bool clicked(false), fading(false);
  clearcolor(Conf::getColor("intro_background"));
  GL::Program guiProgram(GL::Shader(File("Shader/gui.vert"),GL_VERTEX_SHADER),
                         GL::Shader(File("Shader/gui.frag"),GL_FRAGMENT_SHADER));
  gui->place(new GUI::ActionListener(new GUI::Image(Image::Bitmap("Image/logo.png")),[](GUI::ActionListener* al, Dynamic::Var& v, GUI::Event e) {
    if(e.type == GUI::Event::leftClick) {
      *v.as<bool*>() = true;
      return true;
    }
    return false;
  },&clicked),Point2i(0,0),GUI::CC,GUI::CC);
  Wwise::postEvent("General_start");
  while(Window::loop()) {
    while(Window::newEvent(event))
      gui->event(event);
    if(Window::isPressed(Window::Event::ESCAPE))
      Window::close();
    else if(!fading && clicked) {
      fading = true;
      fadeTimer.setoff();
      Game::start();
      Wwise::postEvent("Click_start");
    }
    Wwise::update();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    guiProgram.use();
    guiProgram.uniform("projection",guicam.projection());
    gui->draw(guiProgram);
    UI::drawCursor();
    if(fading) {
      float since(fadeTimer.since().fSeconds());
      float fade(std::min(1.f,since/menuFadeDuration));
      mask(Color::from(0,0,0,fade));
      if(since > menuFadeDuration+introDarkDuration)
        break;
      Game::frame();
      Event::updateAll();
    }
    Window::swapBuffers();
  }
  gui->clear();
}
bool isTumor(Voxel v) {
  return v.type()==Voxel::TUMOR
         ||v.type()==Voxel::TUMOR_IDLE
         ||v.type()==Voxel::TUMOR_CHEMO
         ||v.type()==Voxel::TUMOR_THIRSTY
         ||v.type()==Voxel::TUMOR_THIRSTY_IDLE
         ||v.type()==Voxel::TUMOR_THIRSTY_CHEMO
         ||v.type()==Voxel::VESSEL;
}
bool isChemo(Voxel v) {
  return v.type()==Voxel::TUMOR_CHEMO
         ||v.type()==Voxel::TUMOR_THIRSTY_CHEMO
         || v.type()==Voxel::ORGAN_CHEMO;
}
List<Point3f> burst(float radius, int count) {
  List<Point3f> wtr;
  int i(0);
  if(count) {
    Point3f center, hit;
    Point2f pixelToNormalized(1.f/Window::width(),1.f/Window::height());
    if(world.raycast(cam.position(),cam.screenToRay(Window::normalizedMousePosition()),center,512)) {
      if(isTumor(world.voxel(center.x(),center.y(),center.z())))
        wtr.push_back(center);
      while(wtr.size()<count && ++i<256) {
        hit = center+Point3f::random()*radius;
        if(isTumor(world.voxel(hit.x(),hit.y(),hit.z())))
          wtr.push_back(hit);
      }
    }
  }
  return wtr;
}
void game() {
  fadeTimer.setoff();
  clearcolor(Conf::getColor("background"));
  // Light initialization
  GL::Light light;
  light.position(-1,2,-1,0);
  // Programs initialization
  GL::Program voxelProgram(GL::Shader(File("Shader/voxel.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("Shader/voxel.frag"),GL_FRAGMENT_SHADER));
  GL::Program polyProgram(GL::Shader(File("Shader/poly.vert"),GL_VERTEX_SHADER),
                          GL::Shader(File("Shader/poly.frag"),GL_FRAGMENT_SHADER));
  GL::Program debugProgram(GL::Shader(File("Shader/debug.vert"),GL_VERTEX_SHADER),
                           GL::Shader(File("Shader/debug.frag"),GL_FRAGMENT_SHADER));
  Ref<GL::Program> guiProgram(Resource::program("Shader/gui"));
  Ref<GL::Program> ppProgram(Resource::program("Shader/pp"));
  GL::PostProcess pp(Window::width(),Window::height());
  // Textures fetching
  Ref<GL::Texture> voxelTexture(Resource::texture(Conf::getString("texture_path"),true));
  Ref<GL::Texture> tutorialCamera(Resource::texture("Image/Tutorial/camera.png"));
  Ref<GL::Texture> tutorialTumor(Resource::texture("Image/Tutorial/tumor.png"));
  Ref<GUI::Base> tutorialChemo(new GUI::Image(Resource::texture("Image/Tutorial/chemo.png")));
  // GUI initialization
  Point3f mouseWorld;
  GUI::Text* text(new GUI::Text());
  gui->place(text,Point2i(0,0),GUI::TL,GUI::TL);
  Timer checktimer, clicktimer;
  sca.addBranch(SCA::Branch(NULL,irrigationSphereCenter,Point3f(0,0,0)));
  Coroutine searchCoroutine(search), automataCoroutine(Automaton::updateAll);
  Game::frame();
  while(Window::loop()) {
    float deltaTime(Game::frame().fSeconds());
    movedCamera |= cam.update(world,deltaTime); // Update camera
    world.update(); // Update world
    // Update Wwise
    Wwise::listen(cam.listenerPosition(),cam.forward(),cam.up());
    Wwise::rtpc("Circle_gauge",resource);
    Wwise::rtpc("Time_passing",Game::sinceStart().fSeconds());
    Wwise::rtpc("Random",Rand::nextFloat());
    Wwise::rtpc("Perlin",perlin.value(Game::sinceStart().fSeconds()));
    Wwise::update();
    // Cast mouse ray
    bool mouseHits(world.raycast(cam.position(),cam.screenToRay(Window::normalizedMousePosition()),mouseWorld,512));
    bool canPlaceTumor(mouseHits && world.spherecast(mouseWorld,4,[](Voxel v) {return v.type()==Voxel::ORGAN;}) && sca.distance(mouseWorld,autoaimRadius)<autoaimRadius);
    automataCoroutine.jumpFor(Time(automataTPF*1000000.f));
    searchCoroutine.jumpFor(Time(searchTPF*1000000.f));
    sca.update(world);
    resource = std::min(1.f,resource+deltaTime*((Automaton::has(growth)||Automaton::has(thirst))?resourceSpeed:resourceSpeedIdle));
    if(checktimer.every(Time(0,100))) {
      Bonus::updateAll(world);
      Event::updateAll();
      tumorCount = world.typeCount(Voxel::TUMOR) + world.typeCount(Voxel::TUMOR_IDLE);
      tumorThirstyCount = world.typeCount(Voxel::TUMOR_THIRSTY) + world.typeCount(Voxel::TUMOR_THIRSTY_IDLE);
      if(debugText)
        text->sText("tumor: "+ToString(tumorCount)+"\n"
                    "thirsty: "+ToString(tumorThirstyCount)+"\n"
                    "cursor position: "+ToString((Point3i)mouseWorld)+"\n"
                    "cursor distance: "+ToString(mouseWorld.dist(cam.position()))+"\n"
                    "distance to bonus: "+ToString(Bonus::distanceToInactive(mouseWorld))+"\n"
                    "automata count: "+ToString(Automaton::count())+"\n"
                    "listener distance: "+ToString(cam.listenerDistance())+"\n"
                    "time: "+Time::format("%M:%S",Game::sinceStart())+"\n"
                    "fps: "+ToString(1/deltaTime)+"\n");
      else text->sText("");
    }
    while(Window::newEvent(event)) {
      if(gui->event(event)) continue;
      if(event.type == Window::Event::BUTTONDOWN)
        switch(event.button) {
          case Window::Event::LBUTTON:
            if(true) {
              Automaton* chemoAutomaton(Automaton::get(chemo,mouseWorld));
              if(chemoAutomaton)
                chemoAutomaton->mulTime(.0f);
            }
            break;
          case Window::Event::SPACE:
            if(mouseHits)
              startChemo(mouseWorld);
            break;
          case Window::Event::W:
            displayVessels = !displayVessels;
            break;
          case Window::Event::T:
            displayTargets = !displayTargets;
            break;
          case Window::Event::A:
            displayAutomata = !displayAutomata;
            break;
          case Window::Event::D:
            debugText = !debugText;
            break;
          case Window::Event::NUM1:
            Wwise::postEvent("Voice_event_1");
            break;
          case Window::Event::NUM2:
            Wwise::postEvent("Music_event_1");
            break;
          case Window::Event::NUM8:
            Wwise::postEvent("lol_event");
            break;
          default:
            break;
        }
      cam.event(world,event);
    }
    if(Window::isPressed(Window::Event::LBUTTON) && clicktimer.every(clickLapse)) {
      Point3f hit;
      Point2f pixelToNormalized(1.f/Window::width(),1.f/Window::height());
      if(canPlaceTumor && tumorCost<resource && !Automaton::has(growth,growthCount)
          && world.raycast(cam.position(),cam.screenToRay((Point2f::random()*pixelToNormalized*UI::cursorRadius)+Window::normalizedMousePosition()),hit,512)) {
        placedTumor = true;
        Wwise::postEvent("Tumor_right");
        startGrowth(hit);
        resource -= tumorCost;
      } else Wwise::postEvent("Tumor_wrong"); // Wrong because wrong place
    }
    if(Window::isPressed(Window::Event::ESCAPE) || Game::sinceStart().fSeconds()>gameDuration)
      break;
    glEnable(GL_DEPTH_TEST);
    pp.prerender();
    glMatrixMode(GL_MODELVIEW); // Reset matrix
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Start drawing 3D
    light.set(GL_LIGHT0);
    voxelProgram.use(); // Draw voxels
    voxelProgram.uniform("ambientLevel",ambientLevel);
    voxelProgram.uniform("time",Game::sinceStart().fSeconds());
    voxelProgram.uniform("view",cam.view());
    voxelProgram.uniform("projection",cam.projection());
    voxelProgram.uniform("texture",*voxelTexture,GL_TEXTURE0);
    voxelProgram.uniform("eye",cam.position());
    voxelProgram.uniform("sphereCenter",cam.center());
    voxelProgram.uniform("sphereRadius",cam.radius()*.75f);
    world.draw(cam);
    debugProgram.use(); // Draw debug
    debugProgram.uniform("view",cam.view());
    debugProgram.uniform("projection",cam.projection());
    //GL::Utils::drawAxes();
    if(displayAutomata)
      Automaton::drawAll();
    polyProgram.use();
    polyProgram.uniform("view",cam.view());
    polyProgram.uniform("projection",cam.projection());
    glDisable(GL_DEPTH_TEST);
    if(displayVessels)
      sca.draw();
    if(displayTargets)
      sca.drawTargets();
    pp.postrender(*ppProgram);
    glDisable(GL_DEPTH_TEST); // Start drawing GUI
    guiProgram->use();
    guiProgram->uniform("projection",guicam.projection());
    Bonus::drawAll(*guiProgram,cam);
    switch(tutoStep) { // Draw tutorial
      case 0:
        if(Game::sinceStart()>tutoDelay)
          tutoStep = 1;
        break;
      case 1: // Camera movement
        if(!movedCamera)
          UI::drawTip(*guiProgram,cam,tutorialCamera,tutoCameraPosition);
        else tutoStep = 2;
        break;
      case 2: // Tumor
        if(!placedTumor)
          UI::drawTip(*guiProgram,cam,tutorialTumor,tutoTumorPosition);
        else tutoStep = 3;
        break;
      case 3: // Chemo
        if(Game::sinceStart()>tutoChemoStart) {
          gui->place(tutorialChemo,Point2i(0,0),GUI::BR,GUI::BR);
          tutoStep = 4;
        }
        break;
      case 4: // Chemo end
        if(Game::sinceStart()>tutoChemoEnd) {
          gui->detach(tutorialChemo);
          tutoStep = 5;
        }
        break;
    }
    gui->draw(*guiProgram);
    UI::drawCursor(resource,canPlaceTumor);
    // Fade
    float since(fadeTimer.since().fSeconds());
    float gameSince(Game::sinceStart().fSeconds());
    if(since<gameFadeDuration) {
      float fade(std::min(1.f,since/gameFadeDuration));
      mask(Color::from(0,0,0,1.f-fade));
    } else if(gameSince>gameDuration-gameFadeDuration) {
      float fade(std::min(1.f,(gameSince-(gameDuration-gameFadeDuration))/gameFadeDuration));
      mask(Color::from(0,0,0,fade));
    }
    Window::swapBuffers();
  }
  gui->clear();
}
void credits() {
  Wwise::postEvent("End_display");
  fadeTimer.setoff();
  clearcolor(Color::black);
  Ref<GL::Program> guiProgram(Resource::program("Shader/gui"));
  gui->place(new GUI::Image(Image::Bitmap("Image/credits.png")),Point2i(0,0),GUI::CC,GUI::CC);
  while(Window::loop()) {
    while(Window::newEvent(event))
      gui->event(event);
    if(Window::isPressed(Window::Event::ESCAPE))
      Window::close();
    Wwise::update();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    guiProgram->use();
    guiProgram->uniform("projection",guicam.projection());
    gui->draw(*guiProgram);
    float since(fadeTimer.since().fSeconds());
    float fade(std::min(1.f,since/gameFadeDuration));
    mask(Color::from(0,0,0,1.f-fade));
    Window::swapBuffers();
  }
  gui->clear();
}
int main(int argc, char* argv[]) {
  // Interfaces initialization
  new STB();
  new OBJ();
  new JSON();
  Font::set(new FTFont("Arial.ttf",16));
  // Fetch configuration
  Conf::open("conf.json");
  // Window and OpenGL initialization
  int flags(Conf::getBool("cursor")?0:Window::nocursor);
  if(Conf::getBool("fullscreen"))
    Window::openFullscreen("Apoptosis",flags);
  else Window::open("Apoptosis",Conf::getInt("width"),Conf::getInt("height"),flags);
  // Continue configuration
  Voxel::configure();
  SCA::configure();
  SphericalCamera::configure();
  Game::registerValue("irrigation_radius",&irrigationRadius);
  Game::registerValue("growth_vps",&growthVPS);
  Game::registerValue("growth_duration",&growthDuration);
  Game::registerValue("growth_start_radius",&growthStartRadius);
  Game::registerValue("growth_count",&growthCount);
  Game::registerValue("thirst_vps",&thirstVPS);
  Game::registerValue("thirst_appearance_factor",&thirstAppearanceFactor);
  Game::registerValue("chemo_vps",&chemoVPS);
  Game::registerValue("chemo_propagation_factor",&chemoPropagationFactor);
  Game::registerValue("chemo_organ_factor",&chemoOrganFactor);
  Game::registerValue("chemo_duration",&chemoDuration);
  Game::registerValue("chemo_disappearance_factor",&chemoDisappearanceFactor);
  Game::registerValue("chemo_count",&chemoCount);
  Game::registerValue("budding_vps",&buddingVPS);
  Game::registerValue("budding_duration",&buddingDuration);
  Game::registerValue("budding_factor",&buddingFactor);
  Game::registerValue("budding_max_distance",&buddingMaxDistance);
  Game::registerValue("budding_min_y",&buddingMinY);
  Game::registerValue("budding_count",&buddingCount);
  Game::registerValue("vessel_budding_factor",&vesselBuddingFactor);
  Game::registerValue("chemo_budding_factor",&chemoBuddingFactor);
  Game::registerValue("chemo_budding_curve",&chemoBuddingCurve);
  Game::registerValue("resource_speed",&resourceSpeed);
  Game::registerValue("resource_speed_idle",&resourceSpeedIdle);
  Game::registerValue("tumor_cost",&tumorCost);
  Game::registerValue("vessel_cost",&vesselCost);
  Game::registerValue("autoaim_radius",&autoaimRadius);
  Game::registerValue("vessel_count",&vesselCount);
  Game::registerValue("burst_radius",&burstRadius);
  Bonus::configure();
  Event::configure(&world,&sca);
  automataTPF = Conf::getFloat("automata_tpf");
  searchTPF = Conf::getFloat("search_tpf");
  menuFadeDuration = Conf::getFloat("menu_fade_duration");
  introDarkDuration = Conf::getFloat("intro_dark_duration");
  gameFadeDuration = Conf::getFloat("game_fade_duration");
  gameDuration = Conf::getFloat("game_duration");
  ambientLevel = Conf::getFloat("ambient_level");
  clickLapse = Time(Conf::getFloat("click_lapse")*1000000.f);
  irrigationSphereCenter = Conf::getPoint("irrigation_sphere_center");
  tutoDelay = Time(Conf::getFloat("tuto_delay")*1000000.f);
  tutoChemoStart = Time(Conf::getFloat("tuto_chemo_start")*1000000.f);
  tutoChemoEnd = Time(Conf::getFloat("tuto_chemo_end")*1000000.f);
  tutoCameraPosition = Conf::getPoint("tuto_camera_position");
  tutoTumorPosition = Conf::getPoint("tuto_tumor_position");
  cam.reset(irrigationSphereCenter);
  // GUI initialization
  gui = new GUI::RelativeContainer(Point2i(Window::width(),Window::height()));
  guicam.pixels();
  UI::configure();
  // Wwise initialization
  Wwise::init(AKTEXT("Wwise/"));
  AK::SoundEngine::RegisterPlugin(AkPluginTypeSource, 64, 100, CreateHV_Plug1_WwisePluginEngine, CreateHV_Plug1_WwisePluginEngineParams);
  Wwise::loadBank(AKTEXT("Init.bnk"));
  Wwise::loadBank(AKTEXT("Main.bnk"));
  // OpenGL initialization
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  // Load world first
  File file("world");
  if(file.exists())
    world.read(file.open("rb"));
  else {
    fillObj(Conf::getString("model_path").c_str(),Voxel::ORGAN);
    world.write(file.open("wb"));
  }
  file.close();
  world.updateAll(); // Create VBOs
  // Iterate through phases
  menu();
  game();
  credits();
  // Terminate
  Wwise::term();
  return 0;
}
