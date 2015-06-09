#include "Resource.h"

using namespace L;

L::Map<L::String,L::Ref<L::GL::Texture> > Resource::_textures;
L::Map<L::String,L::Ref<L::GL::Mesh> > Resource::_meshes;
L::Map<L::String,L::Ref<L::GL::Program> > Resource::_programs;

const L::Ref<L::GL::Texture>& Resource::texture(const L::String& path) {
  if(!_textures.has(path))
    return _textures[path] = new GL::Texture(Image::Bitmap(path));
  return _textures[path];
}
const L::Ref<L::GL::Mesh>& Resource::mesh(const L::String& path) {
  if(!_meshes.has(path)) {
    GL::Mesh* m(new GL::Mesh());
    Interface<GL::Mesh>::fromFile(*m,path);
    return _meshes[path] = m;
  }
  return _meshes[path];
}
const L::Ref<L::GL::Program>& Resource::program(const L::String& path) {
  if(!_programs.has(path))
    return _programs[path] = new GL::Program(
      GL::Shader(File(path+".vert"),GL_VERTEX_SHADER),
      GL::Shader(File(path+".frag"),GL_FRAGMENT_SHADER));
  return _programs[path];
}
