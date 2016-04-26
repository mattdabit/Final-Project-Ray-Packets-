#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  BBox bb(p1);
  bb.expand(p2); 
  bb.expand(p3);
  return bb;

}

bool Triangle::intersect(const Ray& r) const {
  
  // TODO Part 1, task 3: implement ray-triangle intersection
  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  
  Vector3D E1 = p2 - p1;
  Vector3D E2 = p3 - p1;
  Vector3D T = r.o - p1;//S
  Vector3D D = r.d;
  Vector3D P = cross(D, E2);//S1

  double det = dot(P, E1);
  double invDet = (1 / det);
  if(invDet == 0.0)
  {
    return false;
  }
  
  Vector3D Q = cross(T, E1);//S2
  double u = dot(T, P)*invDet;
  double v = dot(Q, D)*invDet;
  double w = 1 - u - v;

  if ((u > 0 && u <= 1) && (v > 0 && v <= 1) && (u + v <= 1))
  {
    double t = dot(E2, Q)*invDet;

    //Check if t is with the min and max 
    if (t < r.min_t || t > r.max_t)
    {
      return false;
    }
    r.max_t = t;
    return true;
  }
  else
  {
    return false;
  }
  
}

bool Triangle::intersect(const Ray& r, Intersection *isect) const {
  
  // TODO Part 1, task 3:   
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  Vector3D n1(mesh->normals[v1]), n2(mesh->normals[v2]), n3(mesh->normals[v3]);

  Vector3D E1 = p2 - p1;
  Vector3D E2 = p3 - p1;
  Vector3D T = r.o - p1;//S
  Vector3D D = r.d;
  Vector3D P = cross(D, E2);//S1

  double det = dot(P, E1);
  double invDet = (1 / det);
  if(invDet == 0.0)
  {
    return false;
  }
  Vector3D Q = cross(T, E1);//S2
  double u = dot(T, P)*invDet;
  double v = dot(Q, D)*invDet;
  double w = 1 - u - v;

  if ((u > 0 && u <= 1) && (v > 0 && v <= 1) && (u + v <= 1))
  {
    
    double t = dot(E2, Q)*invDet;
    //Check if t is with the min and max 
    if (t < r.min_t || t > r.max_t)
    {
      return false;
    }
    
    r.max_t = t;
    isect->t = t;
    isect->primitive = this;
    isect->bsdf = get_bsdf();

    //now use w u v to interpolate n1, n2, n3
    Vector3D N = w*n1 + u*n2 + v*n3;
    isect->n = N;
    return true;
  }
  else
  {
    return false;
  }
  
  
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}



} // namespace StaticScene
} // namespace CGL
