#include "Perceptible.h"
#include "Common.h"

#include <tinyxml.h>


vector<RaytracingPrimitive *> PerceptibleContainer::GetRaytracingPrimitives()
{
	vector<Perceptible *> perceptibles= GetPerceptibles();

	vector<RaytracingPrimitive *> primitives;
	for(unsigned int i= 0; i< perceptibles.size(); i++)
	{
		vector<RaytracingPrimitive *> perceptible_primitives= perceptibles[i]->GetRaytracingPrimitives();

		primitives.insert(primitives.end(), perceptible_primitives.begin(), perceptible_primitives.end());
	}

	return primitives;
}

void PerceptibleContainer::Rasterize()
{
	vector<Perceptible *> perceptibles= GetPerceptibles();

	for(unsigned int i= 0; i< perceptibles.size(); i++)
		perceptibles[i]->Rasterize();
}


string Entity::EntityData::MakeFilepath(string filename)
{
	return "data/content/entities/"+ filename;
}

vector<Entity::EntityData *> Entity::EntityData::Parse(string filename)
{
	return Utility::MakeVector(new Entity::EntityData(MakeFilepath(filename)));
}

Entity::EntityData::EntityData(string filepath)
	: FileResource(filepath)
{
	TiXmlDocument perceptual_data;
	perceptual_data.LoadFile(filepath.c_str());

	TiXmlElement *element= perceptual_data.RootElement();
	while(element!= nullptr)
	{
		if(element->ValueTStr()== "perception")
			element= element->FirstChildElement();

		if(element->ValueTStr()== "model")
		{
			TiXmlElement *name= element->FirstChildElement("name");
			TiXmlElement *filename= element->FirstChildElement("filename");
			TiXmlElement *element_name= element->FirstChildElement("element");

			animations[name->GetText()]= new Animation(Mesh::Retrieve(filename->GetText(), element_name->GetText()));
		}

		element= element->NextSiblingElement();
	}
}

Entity::EntityData * Entity::GetEntityData()
{
	return EntityData::Retrieve(GetEntityDataFolderName()+ "/"+ GetEntityDataFilename());
}

MeshProp * Entity::GetMeshProp()
{
	if(mesh_prop== nullptr)
		mesh_prop= new MeshProp(GetEntityData()->animations["default"]->GetMesh());

	mesh_prop->SetDisplacement(GetPosition());

	return mesh_prop;
}

Entity::Entity()
{
	
}

Mesh * Entity::GetIconMesh()
{
	return GetEntityData()->animations["default"]->GetMesh();
}

vector<RaytracingPrimitive *> Entity::GetRaytracingPrimitives()
{
	return GetMeshProp()->GetRaytracingPrimitives();
}

void Entity::Rasterize()
{
	GetMeshProp()->Rasterize();
}