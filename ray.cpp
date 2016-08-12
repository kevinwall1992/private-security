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