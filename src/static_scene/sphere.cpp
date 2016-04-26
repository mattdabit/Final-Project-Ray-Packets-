#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO Part 1, task 4:
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  Vector3D r_o = r.o - o;
  double c = dot(r_o, r_o) - r2;
  double b = dot(2.0*r_o, r.d);
  double a = dot(r.d, r.d);
  double det = pow(b, 2) - 4*a*c;
  if (det <= 0.0)
  {
    return false;
  }
  //sphere hit
  double dis = sqrt(det);
  double t_1 =  (-b + dis)/2*a;
  double t_2 =  (-b - dis)/2*a;

  t1 = min(t_1, t_2);
  t2 = max(t_1, t_2);
  return true;

}

bool Sphere::intersect(const Ray& r) const {

  // TODO Part 1, task 4:
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1;
  double t2;
  bool test1 = test(r, t1, t2);

  if (test1)
  {
    double tmp = t1;
    t1 = min(tmp, t2);
    t2 = max(tmp, t2);
    if (t1 >= 0 && t1 <= r.max_t && t1 >= r.min_t)
    {
      r.max_t = t1;
    }
    else if (t2 >= 0 && t2 <= r.max_t && t2 >= r.min_t){
      r.max_t = t2;
    }
    else
    {
      return false;
    }
    

    return true;
  }
  return false;

}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO Part 1m task 4:
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

  double t1;
  double t2;
  bool test1 = test(r, t1, t2);

  if (test1)
  {
    double tmp = t1;
    t1 = min(tmp, t2);
    t2 = max(tmp, t2);

    if (t1 >= 0 && t1 <= r.max_t && t1 >= r.min_t  && t1 < i->t)
    {
   
      r.max_t = t1;
      i->t = t1;
      i->primitive = this;
      i->bsdf = get_bsdf();
      Vector3D hitpt = r.o + t1*r.d;

      i->n = (hitpt - this->o);
      (i->n).normalize();

    }
    else if (t2 >= 0 && t2 <= r.max_t && t2 >= r.min_t  && t2 < i->t)
    {
      //if t1 is negative & t2 positive ray in sphere
      r.max_t = t2;
      i->t = t2;
      i->primitive = this;
      i->bsdf = get_bsdf();
      Vector3D hitpt = r.o + t2*r.d;
      i->n = (hitpt - this->o);
      (i->n).normalize();

    } 
    else
    {
      return false;
    }

  
    return true;
  }
  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
