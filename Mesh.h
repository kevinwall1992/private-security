#ifndef EBR_MESH
#define EBR_MESH

#include "Common.h"

struct Mesh
{
	vector<float> positions;
	vector<float> texture_coordinates;
	vector<float> normals;

	vector<int> position_indices;
	vector<int> texture_coordinate_indices;
	vector<int> normal_indices;

	Mesh(string filename);
	Mesh();

	int GetTriangleCount();
	int GetVertexCount();

	void LoadOBJ(string filename);
	void Clear();
};

#endif