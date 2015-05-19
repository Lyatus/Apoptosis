#include "UI.h"

using namespace L;

Color UI::cursorPointColor, UI::backgroundDiskColor, UI::innerDiskColor;
GL::Mesh* UI::disk;

void ::UI::drawCursor(float radius, float value) {
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(Window::mousePosition().x(),Window::mousePosition().y(),0);
  GL::whiteTexture().bind();
  GL::color(backgroundDiskColor);
  glPushMatrix();
  glScalef(radius,radius,1);
  disk->draw();
  GL::color(innerDiskColor);
  glScalef(value,value,1);
  disk->draw();
  glPopMatrix();
  GL::color(cursorPointColor);
  disk->draw();
  glPopMatrix();
}
void ::UI::drawCursor() {
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
void ::UI::configure() {
  cursorPointColor = Conf::getColor("cursor_point_color");
  backgroundDiskColor = Conf::getColor("background_disk_color");
  innerDiskColor = Conf::getColor("inner_disk_color");
  disk = new GL::Mesh();
  GL::makeDisc(*disk,32);
}
