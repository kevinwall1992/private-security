#ifndef EBR_SURFACE
#define EBR_SURFACE

#include "Math.h"

struct Surface
{
	Vec3f position;
	Vec3f normal;

	Surface(Vec3f position, Vec3f normal);
	Surface();
};

#endif