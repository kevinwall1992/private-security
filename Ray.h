#ifndef EBR_RAY
#define EBR_RAY

#include "Common.h"
#include "EmbreeSystem.h"
#include "Color.h"
#include "Surface.h"


#define VisibilityRay RTCRay

//need to test compile time packet sizes vs run time packet sizes
#if STREAM_MODE
#define VisibilityRayPacket RTCRayNt<PACKET_SIZE>
#else
#define VisibilityRayPacket JOIN(RTCRay, PACKET_SIZE)
#endif


struct RayType{enum Enum {Primary, Reflection, Refraction, Diffuse};};

__declspec(align(16)) 
struct Ray : public VisibilityRay
{
	float x, y;
	Color absorption;
	int bounce_count;
	RayType::Enum type;

	Surface surface;

	float *light_coefficients;
};

__declspec(align(16)) 
struct RayPacket : public VisibilityRayPacket
{
	float x[PACKET_SIZE], y[PACKET_SIZE];
	float absorption_r[PACKET_SIZE], absorption_g[PACKET_SIZE], absorption_b[PACKET_SIZE];
	int bounce_count[PACKET_SIZE];
	RayType::Enum type[PACKET_SIZE];

	float surface_position_x[PACKET_SIZE];
	float surface_position_y[PACKET_SIZE];
	float surface_position_z[PACKET_SIZE];

	float surface_normal_x[PACKET_SIZE];
	float surface_normal_y[PACKET_SIZE];
	float surface_normal_z[PACKET_SIZE];

	int material_id[PACKET_SIZE];

	float *light_coefficients;
};

#endif