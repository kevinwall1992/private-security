#ifndef EBR_MATH
#define EBR_MATH

#include "embree_util\math\vec3.h"

//Thanks Microsoft
#ifdef GetObject
#undef GetObject
#endif

using embree::Vec3f;
using embree::normalize;
using embree::cross;
using embree::dot;


struct Vec2f
{
	float x, y;

	Vec2f(float x, float y);
	Vec2f();
};

void SetFloat3(float *destination, const Vec3f &vector);
Vec3f MakeVec3f(const float components[3]);

int IntegerPow(int base, int exp);

int GCD(int u, int v);

#endif