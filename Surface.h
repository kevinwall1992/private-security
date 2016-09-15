#ifndef EBR_SURFACE
#define EBR_SURFACE

#include "Math.h"
#include "Material.h"

struct Surface
{
	Vec3f position;
	Vec3f normal;
	Material *material;

	Surface(Vec3f position, Vec3f normal, Material *material);
	Surface();
};

#endif