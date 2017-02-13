#ifndef EBR_MESH
#define EBR_MESH

#include "Common.h"
#include "FileResource.h"
#include "Material.h"

//Consider recreating Prop under a different name;
//Feels wrong to have Material inside Mesh

struct Mesh : public FileResource<Mesh>
{
	Material *material;

	vector<float> positions;
	vector<float> texture_coordinates;
	vector<float> normals;

	vector<int> position_indices;//change to unsigned
	vector<int> texture_coordinate_indices;
	vector<int> normal_indices;

	int GetTriangleCount();
	int GetVertexCount();

	void Clear();

	static string MakeFilepath(string filename);
	static vector<Mesh *> Parse(string filename);

protected:
	Mesh(string filepath, string element_name);
};

#endif