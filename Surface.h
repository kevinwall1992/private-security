#ifndef EBR_SURFACE
#define EBR_SURFACE

#include "EBRParameters.h"
#include "Math.h"
#include "Material.h"

struct Surface
{
	Vec3f position;
	Vec3f normal;
	Vec2f texture_coordinates;

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

	float texture_coordinates_u[PACKET_SIZE];
	float texture_coordinates_v[PACKET_SIZE];

	int material_id[PACKET_SIZE];
};

#endif