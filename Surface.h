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

struct SurfacePacket
{
	float position_x[PACKET_SIZE];
	float position_y[PACKET_SIZE];
	float position_z[PACKET_SIZE];

	float normal_x[PACKET_SIZE];
	float normal_y[PACKET_SIZE];
	float normal_z[PACKET_SIZE];

	int material_id[PACKET_SIZE];
};

#endif