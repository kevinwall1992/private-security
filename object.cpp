#include "Object.h"
#include "Space.h"

Object::Object()
{
}

Tile * Object::GetTile()
{
	return nullptr;
}

Vec3f Object::GetPosition()
{
	return Space::space->GetObjectPosition(this);
}

Entity::Pose Object::GetPose()
{
	return Entity::Pose(Entity::State::None);
}
