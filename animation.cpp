#include "Animation.h"

Animation::Animation(Mesh *mesh_)
{
	mesh= mesh_;
}

Mesh * Animation::GetMesh()
{
	return mesh;
}
