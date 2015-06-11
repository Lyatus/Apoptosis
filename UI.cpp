#include "UI.h"

#include "Resource.h"

using namespace L;

float UI::cursorRadius, UI::crossSize, UI::crossThickness;
Color UI::cursorPointColor, UI::backgroundDiskColor, UI::innerDiskColor, UI::crossColor;
GL::Mesh* UI::disk;
Ref<GL::Texture> UI::cursorRight, UI::cursorWrong;
L::Interval2i UI::resourceBarInt;

void UI::drawCursor(float value, bool can) {
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(Window::mousePosition().x(),Window::mousePosition().y(),0);
  GL::whiteTexture().bind();
  GL::color(backgroundDiskColor);
  glPushMatrix();
  glScalef(cursorRadius,cursorRadius,1);
  disk->draw();
  GL::color(innerDiskColor);
  glScalef(value,value,1);
  disk->draw();
  glPopMatrix();
  GL::color(cursorPointColor);
  disk->draw();
  if(!can) {
    float inner(cursorRadius/(sqrt(2)/crossSize));
    glLineWidth(crossThickness);
    GL::color(crossColor);
    glBegin(GL_LINES);
    glVertex2f(-inner,-inner);
    glVertex2f(inner,inner);
    glVertex2f(inner,-inner);
    glVertex2f(-inner,inner);
    glEnd();
  }
  glPopMatrix();
}
void UI::drawCursor(L::GL::Program& program, bool right) {
  Ref<GL::Texture> tex((right)?cursorRight:cursorWrong);
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(Window::mousePosition().x(),Window::mousePosition().y(),0);
  GL::color(Color::white);
  program.uniform("texture",*tex);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex2f(-tex->width()/2,-tex->height()/2);
  glTexCoord2f(0,1);
  glVertex2f(-tex->width()/2,tex->height()/2);
  glTexCoord2f(1,1);
  glVertex2f(tex->width()/2,tex->height()/2);
  glTexCoord2f(1,0);
  glVertex2f(tex->width()/2,-tex->height()/2);
  glEnd();
  glPopMatrix();
}
void UI::drawCursor() {
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(Window::mousePosition().x(),Window::mousePosition().y(),0);
  GL::color(Color::black);
  glBegin(GL_TRIANGLES);
  glVertex2f(0,0);
  glVertex2f(10,20);
  glVertex2f(20,10);
  glEnd();
  glPopMatrix();
}
void UI::drawTip(GL::Program& program, const GL::Camera& cam, const Ref<GL::Texture>& tex, const Point3f& point) {
  Point2f screenPoint;
  if(cam.worldToScreen(point,screenPoint)) {
    screenPoint = Window::normalizedToPixels(screenPoint);
    GL::color(Color::white);
    program.uniform("texture",*tex);
    glBegin(GL_QUADS);
    glTexCoord2f(0,1);
    glVertex2i(screenPoint.x(),screenPoint.y());
    glTexCoord2f(1,1);
    glVertex2i(screenPoint.x()+tex->width(),screenPoint.y());
    glTexCoord2f(1,0);
    glVertex2i(screenPoint.x()+tex->width(),screenPoint.y()-tex->height());
    glTexCoord2f(0,0);
    glVertex2i(screenPoint.x(),screenPoint.y()-tex->height());
    glEnd();
  }
}
void UI::drawResourceBar(float value) {
  drawBar(resourceBarInt,Color(255,255,255,76),Color::white,value);
}
void UI::drawBar(const L::Interval2i& i, const L::Color& background, const L::Color& fill, float value) {
  float fillx((i.max().x()-i.min().x())*value);
  GL::whiteTexture().bind();
  glBegin(GL_QUADS);
  GL::color(background);
  glVertex2f(i.min().x(),i.min().y());
  glVertex2f(i.min().x(),i.max().y());
  glVertex2f(i.max().x(),i.max().y());
  glVertex2f(i.max().x(),i.min().y());
  GL::color(fill);
  glVertex2f(i.min().x(),i.min().y());
  glVertex2f(i.min().x(),i.max().y());
  glVertex2f(i.min().x()+fillx,i.max().y());
  glVertex2f(i.min().x()+fillx,i.min().y());
  glEnd();
}
void UI::configure() {
  cursorRadius = Conf::getFloat("cursor_radius");
  crossSize = Conf::getFloat("cross_size");
  crossThickness = Conf::getFloat("cross_thickness");
  cursorPointColor = Conf::getColor("cursor_point_color");
  backgroundDiskColor = Conf::getColor("background_disk_color");
  innerDiskColor = Conf::getColor("inner_disk_color");
  crossColor = Conf::getColor("cross_color");
  disk = new GL::Mesh();
  cursorRight = (Resource::texture("Image/cursor_right.png"));
  cursorWrong = (Resource::texture("Image/cursor_wrong.png"));
  GL::makeDisc(*disk,32);
  resourceBarInt.add(Point2i(200,Window::height()-107));
  resourceBarInt.add(Point2i(Window::width()-200,Window::height()-100));
}
