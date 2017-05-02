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

			meshes[name->GetText()]= Mesh::Retrieve(filename->GetText(), element_name->GetText());
		}

		element= element->NextSiblingElement();
	}
}

Entity::EntityData * Entity::GetEntityData()
{
	return EntityData::Retrieve(GetEntityDataFolderName()+ "/"+ GetEntityDataFilename());
}

Mesh * Entity::GetMesh()
{
	return GetEntityData()->meshes["default"];
}

Animation * Entity::GetAnimation()
{
	if(!Utility::Contains(GetEntityData()->animations, GetEntityAnimationName()))
		GetEntityData()->animations[GetEntityAnimationName()]= new Animation(GetMesh(), GetEntityAnimationName(), 0.9f);

	Animation *animation= GetEntityData()->animations[GetEntityAnimationName()];
	animation->SetDisplacement(GetPosition());
	animation->SetRotation(GetRotation());
	animation->SetMoment(GetEntityAnimationMoment());

	return animation;
}

Entity::Entity()
{
	
}

Mesh * Entity::GetIconMesh()
{
	return GetMesh();
}

string Entity::GetEntityAnimationName()
{
	return "";
}

float Entity::GetEntityAnimationMoment()
{
	return 0.0f;
}

vector<RaytracingPrimitive *> Entity::GetRaytracingPrimitives()
{
	return GetAnimation()->GetRaytracingPrimitives();
}

void Entity::Rasterize()
{
	GetAnimation()->Rasterize();
}