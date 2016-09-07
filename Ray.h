#ifndef EBR_RAY
#define EBR_RAY

#include "Common.h"
#include "EmbreeSystem.h"
#include "Color.h"


#define Ray RTCRay

//need to test compile time packet sizes vs run time packet sizes
#if STREAM_MODE
#define RayPacket RTCRayNt<PACKET_SIZE>
#else
#define RayPacket JOIN(RTCRay, PACKET_SIZE)
#endif


struct RayType{enum Enum {Primary, Reflection, Refraction, Indirect};};

struct RayExtras
{
	float x, y;
	Color absorption;
	int bounce_count;
	RayType::Enum type;

	RayExtras(float x, float y, Color absorption, int bounce_count, RayType::Enum type);
	RayExtras();
};

//RayPacketExtras or RayExtrasPacket?
struct RayPacketExtras
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
};

struct CompleteRay
{
	Ray *ray;
	RayExtras *extras;

	CompleteRay(Ray *ray, RayExtras *extras);
};

struct CompleteRayPacket
{
	RayPacket *ray_packet;
	int ray_index;

	RayPacketExtras *extras;

	CompleteRayPacket(RayPacket *ray_packet, RayPacketExtras *ray_extras);
};

#endif