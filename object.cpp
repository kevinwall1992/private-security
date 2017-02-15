#include "Object.h"
#include "Space.h"

Object::Object()
{
}

Tile * Object::GetTile()
{
	Vec3i position= GetPosition();

	return &Space::space->tiles[position.x][position.y][position.z];
}

Vec3f Object::GetPosition()
{
	return Space::space->GetObjectPosition(this);
}

Entity::Pose Object::GetPose()
{
	return Entity::Pose(Entity::State::None);
}
