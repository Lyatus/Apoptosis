#ifndef DEF_Cancer_Conf
#define DEF_Cancer_Conf

#include <L/L.h>

class Conf {
  private:
    static L::Var _json;
  public:
    static void open(const L::String&);
    static void load(const char*);
    static const L::Var& get();
    static L::String getString(const L::String&);
    static L::String getStringFrom(const L::Var&);
    static L::Color getColor(const L::String&);
    static L::Point3f getPoint(const L::String&);
    static L::Point3f getPointFrom(const L::Var&);
    static float getFloat(const L::String&);
    static float getFloatFrom(const L::Var&);
    static int getInt(const L::String&);
    static bool getBool(const L::String&);
};

#endif



