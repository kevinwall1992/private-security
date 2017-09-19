#include "Physical.h"

Vec3f Physical::GetPosition()
{
	return position;
}

float Physical::GetRotation()
{
	return rotation;
}

void Physical::SetPosition(Vec3f position_)
{
	position= position_;
}

void Physical::SetRotation(float rotation_)
{
	rotation= rotation_;
}

void Physical::ApplyForce(Vec3f force, Chronons duration)
{
	velocity+= force* ChrononsToSeconds(duration);
}

void Physical::Step(Chronons chronons)
{
	position+= velocity* ChrononsToSeconds(chronons);
}
