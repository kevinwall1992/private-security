#ifndef EBR_MATERIAL
#define EBR_MATERIAL

#include "Common.h"
#include "Color.h"
#include "Math.h"
#include "FileResource.h"

struct Material : public FileResource<Material>
{
	static vector<Material *> GetMaterials();//Would be nice if this was done for us by Resource/ResourceSystem

	static string MakeFilepath(string filename);
	static vector<Material *> Parse(string filename);

protected:
	Material(string filepath, string element_name);

	static vector<Material *> materials_;
};

struct PhongMaterial : public Material
{
	Color diffuse;

	Color specular;
	float glossiness;

	float reflectivity;

	float refractive_index;
	float transparency;

	friend Material;//ewwwww

protected:
	PhongMaterial(string filepath, string element_name);
};

#endif