#ifndef EBR_PROP
#define EBR_PROP

#include "Mesh.h"
#include "Material.h"


struct Prop
{
	Mesh *mesh;
	Material *material;

	Prop(Mesh *mesh, Material *material);
	Prop();

	static vector<Prop> ParseOBJ(string filename);
};



#endif