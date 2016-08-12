#ifndef EBR_MATH
#define EBR_MATH

#include "embree_util\math\vec3.h"

using embree::Vec3f;
using embree::normalize;
using embree::cross;
using embree::dot;


void SetFloat3(float *destination, const Vec3f &vector);
Vec3f MakeVec3f(const float components[3]);

#endif