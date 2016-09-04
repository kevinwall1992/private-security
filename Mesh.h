#ifndef EBR_MESH
#define EBR_MESH

#include "Common.h"
#include "Resource.h"

struct Mesh : public Resource
{
	vector<float> positions;
	vector<float> texture_coordinates;
	vector<float> normals;

	vector<int> position_indices;
	vector<int> texture_coordinate_indices;
	vector<int> normal_indices;

	Mesh(string name);

	int GetTriangleCount();
	int GetVertexCount();

	void Clear();
};

#endif