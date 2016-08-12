#ifndef EBR_RAY
#define EBR_RAY

#include "Common.h"
#include "EmbreeSystem.h"
#include "Color.h"


#define Ray RTCRay
#define RayPacket JOIN(RTCRay, PACKET_SIZE)//need to test compile time packet sizes vs run time packet sizes

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

	RayExtras *extras;

	CompleteRayPacket(RayPacket *ray_packet, int ray_index, RayExtras *ray_extras);
};

#endif