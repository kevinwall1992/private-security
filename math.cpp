#include "Math.h"


void SetFloat3(float *destination, const Vec3f &vector)
{
	destination[0]= vector[0];
	destination[1]= vector[1];
	destination[2]= vector[2];
}

//Need to decide on Make/Construct/Build/etc for these sorts of functions
Vec3f MakeVec3f(const float components[3])
{
	return Vec3f(components[0], components[1], components[2]);
}

Vec2f::Vec2f(float x_, float y_)
{
	x= x_;
	y= y_;
}

Vec2f::Vec2f()
{
	x= 0.0f;
	y= 0.0f;
}