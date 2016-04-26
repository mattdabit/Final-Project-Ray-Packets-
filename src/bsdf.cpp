#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return albedo * (1.0 / PI);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi = sampler.get_sample(pdf);
  return albedo * (1.0 / PI);
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO Part 5:
  // Implement MirrorBSDF
  *pdf = 1.0;
  reflect(wo, wi);
  if (wo.z > 0)
  {
    return reflectance * (1.0/wo.z);
  }
  else
  {
    return Spectrum();
  }
}

// Glossy BSDF //

/*
Spectrum GlossyBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlossyBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0f;
  return reflect(wo, wi, reflectance);
}
*/

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO Part 5:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO Part 5:
  // Compute Fresnel coefficient and either reflect or refract based on it.
  
  Vector3D n = Vector3D(0.0, 0.0, 1.0);
  float n1;
  float n2;

  Vector3D l = -wo;
  float cost1 = dot(-n, l);

  //wo trying to enter material if wo.z is +
  float r;
  if(wo.z > 0.0)
  {
    //1 is for air
    n2 = ior; //other ior
    n1 = 1.0; //ior for wo

  }
  //wo in material, will either exit or total internal reflect
  //Exit material if no total internal reflection
  // indicated by when the radicand for cost2 is negative
  else 
  {
    n2 = 1.0;//other ior
    n1 = ior; //ior for wo
    //since cost1 was negative redo eqn
  }
  
  if(wo.z < 0.0)
  {
    n = Vector3D(0.0 ,0.0 ,-1.0);
    cost1 = dot(-n, l);
  }
  r = (n1/n2);

  float sint2 = r * sqrt(1.0 - (cost1*cost1));
  float cost2 = sqrt(1.0 - (sint2 * sint2));

  //Refract return false if total internal reflection
  //also stores wha we need for wi so no need to do it again
  if (refract(wo, wi, this->ior))
  {
    //Compute refection coefficient
    //Schlick Approx 
    float R0;
    float R;


    R0 = pow(((n1 - n2) / (n1 + n2)), 2.0);
    R = R0 + ((1.0 - R0)*pow((1.0 - cost1), 5.0));

    if (coin_flip(R))
    {
      //Reflection
      reflect(wo, wi);
      *pdf = R;

      return  R*(reflectance* (1.0/(cost1)));
    }
    else
    {
      //refraction
      *pdf = 1.0 - R;
      return (((1.0 - R) * (this->transmittance * ((n2 * n2) / (n1 * n1))))  * (1.0 / cost1));
    }
  }
  else
  {
    //this means ray is in material 
    reflect(wo, wi);
    *pdf = 1.0;
    return reflectance * (1.0/(cost1));
  }
 
}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {

  // TODO Part 5:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  Vector3D n = Vector3D(0.0, 0.0, 1.0);
  Vector3D l = -wo;
  float cost1 = dot(-n, l);
  if (cost1 < 0)
  {
    n = Vector3D(0,0,-1.0);
    cost1 = dot(-n, l);
  }
  *wi = l + (2.0*cost1*n);

}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {

  // TODO Part 5:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  
  // wo is w_out ray going out 
  Vector3D n = Vector3D(0.0, 0.0, 1.0);
  float n1;
  float n2;

  Vector3D l = -wo;
  float cost1 = dot(-n, l);

  //wo Entering material if +
  float r;
  if(wo.z > 0.0)
  {
    //1 is for air
    n2 = ior; //other ior
    n1 = 1.0; //ior for wo

  }
  else //wo in material, will either exit or total internal reflect
  {
    n2 = 1.0;//other ior
    n1 = ior; //ior for wo
  }

  if(wo.z < 0.0)
  {
    n = Vector3D(0.0 ,0.0 ,-1.0);
    cost1 = dot(-n, l);
  }
  r = (n1/n2);

  float sint2 = r * sqrt(1.0 - (cost1*cost1));
  float cost2 = sqrt(1.0 - (sint2 * sint2));
  float sint1 = sqrt(1.0 - (cost1*cost1));

  if ((1.0 - (sint2 * sint2)) < 0.0)
  {
    return false;
  }
  *wi = r*l + ((r*cost1 - cost2)*n);
  
  return true;

}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL
