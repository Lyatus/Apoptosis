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
    static L::Interval2i resourceBarInt;
  public:
    static void drawCursor(float value,bool);
    static void drawCursor(L::GL::Program&,bool);
    static void drawCursor();
    static void drawTip(L::GL::Program&, const L::GL::Camera&, const L::Ref<L::GL::Texture>&, const L::Point3f&);
    static void drawResourceBar(float);
    static void drawBar(const L::Interval2i&, const L::Color& background, const L::Color& fill, float value);
    static void configure();
};

#endif




