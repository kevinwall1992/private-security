#include "Material.h"
#include "System.h"

vector<Material *> Material::materials;
Material * Material::default_material= nullptr;

vector<Material *> Material::GetMaterials()
{
	return materials;
}

Material * Material::GetDefaultMaterial()
{
	if(default_material== nullptr)
		default_material= new PhongMaterial("material.default", Color(0.7f, 0.7f, 0.7f), Color(1.0f, 1.0f, 1.0f), 100.0f, 0);

	return default_material;
}

Material::Material(string name_)
	: Resource(name_)
{
	materials.push_back(this);
}

Material::~Material()
{
	for(unsigned int i= 0; i< materials.size(); i++)
		if(materials[i]->GetName()== this->GetName())
			materials.erase(materials.begin()+ i);
}

Material * Material::GetMaterial(string name)
{
	return dynamic_cast<Material *>(System::resource.GetResource(name));
}


PhongMaterial::PhongMaterial(string name_, 
							 Color diffuse_, 
							 Color specular_, float glossiness_, 
							 float index_of_refraction_)
	: Material(name_)
{
	diffuse= diffuse_;
	specular= specular_;
	glossiness= glossiness_;
	index_of_refraction= index_of_refraction_;
}

PhongMaterial::PhongMaterial(string name_)
	: Material(name_)
{
	
}
	