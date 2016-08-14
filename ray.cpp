#include "Ray.h"

RayExtras::RayExtras(float x_, float y_, Color absorption_, int bounce_count_, RayType::Enum type_)
{
	x= x_;
	y= y_;
	absorption= absorption_;
	bounce_count= bounce_count_;
	type= type_;
}

RayExtras::RayExtras()
{

}

CompleteRay::CompleteRay(Ray *ray_, RayExtras *extras_)
{
	ray= ray_;
	extras= extras_;
}


CompleteRayPacket::CompleteRayPacket(RayPacket *ray_packet_, RayPacketExtras *ray_extras_)
{
	ray_packet= ray_packet_;
	extras= ray_extras_;
}