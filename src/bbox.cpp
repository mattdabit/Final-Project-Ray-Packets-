#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO Part 2, task 2:
  // Implement ray - bounding box intersection test
  // If the ray intersected the bounding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  double min_x = min.x;
  double min_y = min.y;
  double min_z = min.z;

  double max_x = max.x;
  double max_y = max.y;
  double max_z = max.z;

  double new_t0_x = (min_x - r.o.x)/r.d.x;
  double new_t0_y = (min_y - r.o.y)/r.d.y;
  double new_t0_z = (min_z - r.o.z)/r.d.z;

  double new_t1_x = (max_x - r.o.x)/r.d.x;
  double new_t1_y = (max_y - r.o.y)/r.d.y;
  double new_t1_z = (max_z - r.o.z)/r.d.z;

  min_x = std::min(new_t0_x, new_t1_x);
  min_y = std::min(new_t0_y, new_t1_y);
  min_z = std::min(new_t0_z, new_t1_z);

  max_x = std::max(new_t0_x, new_t1_x);
  max_y = std::max(new_t0_y, new_t1_y);
  max_z = std::max(new_t0_z, new_t1_z);

  double min_t = std::max(min_x, std::max(min_y, min_z));//get closest
  double max_t = std::min(max_x, std::min(max_y, max_z));

  if (min_t > max_t)
  {
    return false;
  }

  t0 = min_t;
  t1 = max_t;
  return true;
}

void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
