#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c);
  } else {
    draw(node->l, c);
    draw(node->r, c);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c);
  } else {
    drawOutline(node->l, c);
    drawOutline(node->r, c);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // TODO Part 2, task 1:
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  //compute bbox of primitives
  BBox centroid_box, bbox;
  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c); 
  }

  //init node with bbox
  BVHNode *node = new BVHNode(bbox);
  //base case found leaf
  if (prims.size() <= max_leaf_size) 
  {
    //allocate new vector prim for the prim list return node
    node->prims = new vector<Primitive *>(prims);
    node->l = NULL;
    node->r = NULL;
    return node;
  }

  //Max dimension of extent
  double max_dim = max(max(bbox.extent.x, bbox.extent.y), bbox.extent.z);
  //split pt of bounding box 
  Vector3D split_pt = centroid_box.centroid();
  //left and right vectors that we recurse on
  vector<Primitive *> left; // may need to do different type of allocating left may need to be pter
  vector<Primitive *> right;

  //now for every prim see if it goes left or right
  for (Primitive *p : prims)
  {

    Vector3D c = p->get_bbox().centroid();
    //x axis is highest dimension
    if (bbox.extent.x == max_dim) 
    {
      //need to split prims into left and right vectors
      // if centroid axis val is less than or equal to max_dim it goes to left vector
      if (c.x <= split_pt.x)
      {
        left.push_back(p);
      }
      else
      {
        right.push_back(p);
      }
    }
    //y axis is highest dimension
    else if (bbox.extent.y == max_dim) 
    {
      if (c.y <= split_pt.y)
      {
        left.push_back(p);
      }
      else
      {
        right.push_back(p);
      }
    }
    //z axis is highest dimension
    else if (bbox.extent.z == max_dim) 
    {
      if (c.z <= split_pt.z)
      {
        left.push_back(p);
      }
      else
      {
        right.push_back(p);
      }
    }
  }

  //all elems in prim should now be in left or right vectors
  //now make recursive calles but check if left/right is empty

  if (!left.empty() && !right.empty())
  {
    //assign this new node a left & right node
    node->prims = NULL;
    node->l = construct_bvh(left, max_leaf_size);
    node->r = construct_bvh(right, max_leaf_size);
  }
  else if (!left.empty() && right.empty())
  {
    //assign  left vector as prim
    //allocate new vector prim for the prim list return node
    node->prims = new vector<Primitive *>(left);
    node->l = NULL;
    node->r = NULL;
    return node;
  }
  else if (!right.empty() && left.empty())
  {
    //assign  right vector as prim
    //allocate new vector prim for the prim list return node
    node->prims = new vector<Primitive *>(right);
    node->l = NULL;
    node->r = NULL;
    return node;
  }

  return node;
}

bool BVHAccel::intersect(const Ray& r, BVHNode *node) const {

  // TODO Part 2, task 3:
  // Implement BVH intersection.
  // Currently, we just naively loop over every primitive.
  //just call the second function maybe inefficient
  Intersection* isect = new Intersection();

  return intersect(r, isect, node);

}

bool BVHAccel::intersect(const Ray& r, Intersection* i, BVHNode *node) const {

  // TODO Part 2, task 3:
  // Implement BVH intersection.
  // Currently, we just naively loop over every primitive.

  bool hit = false;
  double t0;
  double t1;
  BBox bbox = node->bb;
  // bool active_ray = any_active_intersect(raypack, node);

  if (!bbox.intersect(r, t0, t1) )
  {
    return false;
  }

  if (node->isLeaf())
  {

    for (Primitive *p : *(node->prims)) 
    {

      total_isects++;
      Intersection* isect = new Intersection();
      hit = p->intersect(r, isect);

      if (hit && isect->t <= i->t)
      {
        r.max_t = isect->t;
        i->n = isect->n;
        i->bsdf = isect->bsdf;
        i->primitive = isect->primitive;
        i->t = isect->t;
      }
    
      free(isect);
    }
  }

  if (node->l)
  {
    intersect(r, i, node->l);
  }
  
  if (node->r)
  {
    intersect(r, i, node->r);
  }


  return !(i->primitive == NULL);
}






//remove const
bool BVHAccel::intersect_raypack(RayPacket& rays, BVHNode *node) const {

  // TODO Part 2, task 3:
  // Implement BVH intersection.
  // Currently, we just naively loop over every primitive.
  //just call the second function maybe inefficient
  Intersection* isect = new Intersection();

  return intersect_raypack(rays, isect, node);

}

//remove const
bool BVHAccel::intersect_raypack(RayPacket& raypack, Intersection* i, BVHNode *node) const {

  // TODO Part 2, task 3:
  // Implement BVH intersection.
  // Currently, we just naively loop over every primitive.

  bool hit = false;
  // double t0;
  // double t1;
  // BBox bbox = node->bb;
  bool active_ray = any_active_intersect(raypack, node);
  // !bbox.intersect(rays, t0, t1) 
  if (!active_ray)
  {
    return false;
  }

  if (node->isLeaf())
  {

    for (Primitive *p : *(node->prims)) 
    {
      for (int j = 0; j < raypack.rays.size(); ++j)
      {
        if (raypack.rays[j].active == true)
        {
          total_isects++;
          Intersection* isect = new Intersection();
          hit = p->intersect(raypack.rays[j], isect);

          if (hit && isect->t <= i->t)
          {
            raypack.rays[j].max_t = isect->t;
            i->n = isect->n;
            i->bsdf = isect->bsdf;
            i->primitive = isect->primitive;
            i->t = isect->t;
          }
        
          free(isect);
        }
      }
        // total_isects++;
        // Intersection* isect = new Intersection();
        // hit = p->intersect(ray, isect);

        // if (hit && isect->t <= i->t)
        // {
        //   ray.max_t = isect->t;
        //   i->n = isect->n;
        //   i->bsdf = isect->bsdf;
        //   i->primitive = isect->primitive;
        //   i->t = isect->t;
        // }
        
        // free(isect);
    }
  }

  if (node->l)
  {
    intersect_raypack(raypack, i, node->l);
  }
  
  if (node->r)
  {
    intersect_raypack(raypack, i, node->r);
  }


  return !(i->primitive == NULL);
}

bool BVHAccel::any_active_intersect(RayPacket& raypack, BVHNode *node) const {
  
  
  // bool is_true;
  // bool ret_val = false;
  // for (int i = 0; i < raypack.rays.size(); ++i)
  // parallel_for_each (begin(raypack.rays), end(raypack.rays), [&](Ray r)
  BBox bbox = node->bb;
  #pragma omp parallel for:
  {
    for (int i = 0; i < raypack.rays.size(); ++i){

      double t0;
      double t1;
      
      if (bbox.intersect(raypack.rays[i], t0, t1))
      {
        raypack.rays[i].active = true;
      }
      else
      {
        raypack.rays[i].active = false;
      }
    // is_true = bbox.intersect(raypack.rays[i], t0, t1); 
    // is_true = bbox.intersect(r, t0, t1); 
    //if a ray in raypacket intersects bbox then its active else its not active
    // if(is_true)
    // {
    //   raypack.active[i] = true;
    //   ret_val = true;
    // }
    // else
    // {
    //   raypack.active[i] = false;
    }

  }
  // return ret_val;
  for (int i = 0; i < raypack.rays.size(); ++i)
  {
    if (raypack.rays[i].active == true)
    {
      return true;
    }
  }
  return false;
}



}  // namespace StaticScene
}  // namespace CGL
