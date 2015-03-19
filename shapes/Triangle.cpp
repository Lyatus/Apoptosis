#include "Triangle.h"

using namespace L;

Triangle::Triangle(const L::Point3f& a, const L::Point3f& b, const L::Point3f& c, float thickness)
  : Shape(true), _ab(a,b,thickness), _bc(b,c,thickness), _ac(a,c,thickness), _thickness(thickness) {
  Point3f newx(c-a), newy(b-a), newz(newx.cross(newy));
  //newx /= pow(newx.norm(),2.f); // Unit vectors to go into triangle space
  //newy /= pow(newy.norm(),2.f);
  newz.normalize(); // Keep world space for z axis (to compute distance from plane)
  _triangleMatrix = Matrix44f::orientation(newx,newy,newz).inverse()*Matrix44f::translation(-a);
  //std::cout << a.x() << " " << a.y() << " " << a.z() << " & " << b.x() << " " << b.y() << " " << b.z() << " & " << c.x() << " " << c.y() << " " << c.z() << " - " << ((newx).dot(newy)) << std::endl;
}
L::Point3i Triangle::start() const {
  return _ab.start();
}
float Triangle::value(const Point3f& p) const {
  Point4f p2(_triangleMatrix*p);
  //std::cout << p.x() << " " << p.y() << " " << p.z() << " -> " << p2.x() << " " << p2.y() << " " << p2.z() << std::endl;
  if(p2.x()<0) // Closer to the ab edge
    return _ab.value(p);
  else if(p2.y()<0) // Closer to the ac edge
    return _ac.value(p);
  else if(p2.x()+p2.y()<=1) // Inside the triangle
    return fromDistance(std::abs(p2.z())-_thickness);
  else // Closer to the bc edge
    return _bc.value(p);
}
