#ifndef DEF_Cancer_UI
#define DEF_Cancer_UI

#include <L/L.h>
#include "Conf.h"

class UI {
  private:
    static float cursorRadius;
    static L::Color cursorPointColor, backgroundDiskColor, innerDiskColor;
    static L::GL::Mesh* disk;
    static L::Ref<L::GL::Texture> cursorRight, cursorWrong;
  public:
    static void drawCursor(float value);
    static void drawCursor(L::GL::Program&,bool);
    static void drawCursor();
    static void drawTip(L::GL::Program&, const L::GL::Camera&, const L::Ref<L::GL::Texture>&, const L::Point3f&);
    static void configure();
};

#endif




