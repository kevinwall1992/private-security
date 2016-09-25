#ifndef EBR_MATERIAL
#define EBR_MATERIAL

#include "Common.h"
#include "Color.h"
#include "Math.h"
#include "Resource.h"

struct Material : public Resource
{
	Material(string name);
	~Material();

	Material * GetMaterial(string name);

	static vector<Material *> GetMaterials();
	static Material * GetDefaultMaterial();

private:
	static vector<Material *> materials;
	static Material *default_material;
};

struct PhongMaterial : public Material
{
	Color diffuse;

	Color specular;
	float glossiness;

	float reflectivity;

	float refractive_index;
	float transparency;


	PhongMaterial(string name, 
		          Color diffuse,
				  Color specular, float glossiness, 
				  float reflectivity,
				  float refractive_index, float transparency);

	PhongMaterial(string name);
};

#endif