#include "Object.h"
#include "Space.h"
#include "GameSystem.h"

Object::Object()
{
}

Tile * Object::GetTile()
{
	Vec3i tile_position= System::game.space.GetObjectPosition(this);

	return &System::game.space.tiles[tile_position.x][tile_position.y][tile_position.z];
}

Vec3f Object::GetPosition()
{
	return System::game.space.GetObjectPosition(this);
}

void Object::Step(Chronons chronons)
{
	HasPose::Step(chronons);
}
