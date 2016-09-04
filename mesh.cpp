#include "Mesh.h"


Mesh::Mesh(string name_)
	: Resource(name_)
{
	Clear();
}

int Mesh::GetTriangleCount()
{
	return (int)(position_indices.size()/ 3);
}

int Mesh::GetVertexCount()
{
	return (int)(positions.size()/ 3);
}

void Mesh::Clear()
{
	positions.clear();
	texture_coordinates.clear();
	normals.clear();

	position_indices.clear();
	texture_coordinate_indices.clear();
	normal_indices.clear();
}