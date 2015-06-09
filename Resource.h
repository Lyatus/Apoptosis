#ifndef DEF_Cancer_Resource
#define DEF_Cancer_Resource

#include <L/L.h>

class Resource {
  private:
    static L::Map<L::String,L::Ref<L::GL::Texture> > _textures;
    static L::Map<L::String,L::Ref<L::GL::Mesh> > _meshes;
    static L::Map<L::String,L::Ref<L::GL::Program> > _programs;
  public:
    static const L::Ref<L::GL::Texture>& texture(const L::String&);
    static const L::Ref<L::GL::Mesh>& mesh(const L::String&);
    static const L::Ref<L::GL::Program>& program(const L::String&);
};

#endif






