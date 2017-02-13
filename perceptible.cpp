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
	return MakeVector(new Entity::EntityData(MakeFilepath(filename)));
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


Entity::Pose::Pose(State::Enum state_, float time_)
{
	state= state_;
	time= time_;
}

Entity::EntityData * Entity::GetEntityData()
{
	return EntityData::Retrieve(GetEntityDataFolderName()+ "/"+ GetEntityDataFilename());
}

Entity::Entity()
{
	
}

//Raytracing and Rasterization methods very similar; might want to abstract their implementations
vector<RaytracingPrimitive *> Entity::GetRaytracingPrimitives()
{
	return MakeVector<RaytracingPrimitive *>(new RaytracingMesh(AreDrawFlagsActive(DrawFlags::RasterizeGbuffers) ? false : true, 
																GetEntityData()->animations["default"]->GetMesh()));
}

void Entity::Rasterize()
{
	if(mesh_prop== nullptr)
		mesh_prop= new MeshProp(GetEntityData()->animations["default"]->GetMesh());
	mesh_prop->SetDisplacement(GetPosition());
	mesh_prop->Rasterize();
}