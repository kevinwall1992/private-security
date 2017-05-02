#include "Physical.h"

Vec3f Physical::GetPosition()
{
	return position;
}

float Physical::GetRotation()
{
	return rotation;
}

void Physical::ApplyForce(Vec3f force, Chronons duration)
{
	velocity+= force* ChrononsToSeconds(duration);
}

void Physical::Step(Chronons chronons)
{
	position+= velocity* ChrononsToSeconds(chronons);
}
