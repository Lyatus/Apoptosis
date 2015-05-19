#ifndef DEF_Cancer_UI
#define DEF_Cancer_UI

#include <L/L.h>
#include "Conf.h"

class UI {
  private:
    static L::Color cursorPointColor, backgroundDiskColor, innerDiskColor;
    static L::GL::Mesh* disk;
  public:
    static void drawCursor(float radius, float value);
    static void drawCursor();
    static void configure();
};

#endif




