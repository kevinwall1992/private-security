#ifndef PS_RAY
#define PS_RAY

#include "Vector.h"

struct Ray
{
	Vec3f tail;
	Vec3f direction;

	Ray(Vec3f tail, Vec3f direction);
};

#endif