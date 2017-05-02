#include "Object.h"
#include "Space.h"
#include "GameSystem.h"

Object::Object()
{
}

Tile * Object::GetTile()
{
	return System::game.space.GetTile(GetPosition());
}

Vec3f Object::GetPosition()
{
	return System::game.space.GetPosition(this);
}

float Object::GetRotation()
{
	return 0.0f;
}

void Object::Step(Chronons chronons)
{
	HasPose::Step(chronons);
}
