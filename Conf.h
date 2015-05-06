#ifndef DEF_Cancer_Conf
#define DEF_Cancer_Conf

#include <L/L.h>

class Conf {
  private:
    static L::Dynamic::Var _json;
  public:
    static void open(const L::String&);
    static L::String getString(const L::String&);
    static L::Color getColor(const L::String&);
    static L::Point3f getPoint(const L::String&);
    static float getFloat(const L::String&);
    static int getInt(const L::String&);
    static bool getBool(const L::String&);
};

#endif



