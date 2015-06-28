#ifndef DEF_Cancer_Conf
#define DEF_Cancer_Conf

#include <L/L.h>

class Conf {
  private:
    static L::Dynamic::Var _json;
  public:
    static void open(const L::String&);
    static void load(const char*);
    static const L::Dynamic::Var& get();
    static L::String getString(const L::String&);
    static L::String getStringFrom(const L::Dynamic::Var&);
    static L::Color getColor(const L::String&);
    static L::Point3f getPoint(const L::String&);
    static L::Point3f getPointFrom(const L::Dynamic::Var&);
    static float getFloat(const L::String&);
    static float getFloatFrom(const L::Dynamic::Var&);
    static int getInt(const L::String&);
    static bool getBool(const L::String&);
};

#endif



