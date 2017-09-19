#include "Perceptible.h"
#include "Common.h"
#include "Actor.h"

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


Entity::EntityData::EntityData()
{
	
}


Mesh * Entity::GetDefaultMesh()
{
	return GetMesh("default");
}

Animation * Entity::GetAnimation()
{
	if(!Utility::Contains(entity_data.animations, GetEntityAnimationName()))
		entity_data.animations[GetEntityAnimationName()]= new Animation(GetDefaultMesh(), GetEntityAnimationName(), 0.9f);

	Animation *animation= entity_data.animations[GetEntityAnimationName()];
	animation->SetDisplacement(GetPosition()+ Vec3f(0.5f, 0.0f, 0.5f));
	animation->SetRotation(GetRotation());
	animation->SetMoment(GetEntityAnimationMoment());
	if(Utility::IsType<Actor>(this))
		animation->RemoveDrawFlags(DrawFlags::Indirect);

	return animation;
}

void Entity::LoadXML(TiXmlElement *xml_element)
{
	TiXmlElement *entity_element= xml_element->FirstChildElement("entity");
	TiXmlElement *child_element= entity_element->FirstChildElement();
	
	while(child_element!= nullptr)
	{
		if(child_element->ValueTStr()== "mesh")
		{
			string name= child_element->FirstChildElement("name")->GetText();
			string filename= child_element->FirstChildElement("filename")->GetText();
			string element_name= child_element->FirstChildElement("element")->GetText();

			entity_data.meshes[name]= Mesh::Retrieve(filename, element_name);
		}
		
		child_element= child_element->NextSiblingElement();
	}
}

Entity::Entity()
{
	
}

Mesh * Entity::GetIconMesh()
{
	return GetDefaultMesh();
}

Mesh * Entity::GetMesh(string name)
{
	return entity_data.meshes[name];
}

std::map<string, Mesh*> Entity::GetMeshes()
{
	return entity_data.meshes;
}

void Entity::SetMesh(string name, Mesh *mesh)
{
	entity_data.meshes[name]= mesh;

	if(name== "default")
	{
		for(std::pair<string, Animation *> animation_pair : entity_data.animations)
			delete animation_pair.second;

		entity_data.animations.clear();
	}
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
	Animation *animation= GetAnimation();

	if(animation== nullptr)
		return vector<RaytracingPrimitive *>();
	else
		return animation->GetRaytracingPrimitives();
}

void Entity::Rasterize()
{
	Animation *animation= GetAnimation();

	if(animation!= nullptr)
		animation->Rasterize();
}

TiXmlElement * Entity::EncodeXML()
{
	TiXmlElement *xml_element= Encodable::EncodeXML();

	TiXmlElement *entity_element= new TiXmlElement("entity");

	for(auto const &mesh_pair: entity_data.meshes)
	{
		string name= mesh_pair.first;
		Mesh *mesh= mesh_pair.second;

		TiXmlElement *mesh_element= new TiXmlElement("mesh");

		TiXmlElement *name_element= new TiXmlElement("name");
		TiXmlText *text= new TiXmlText(name.c_str());
		name_element->LinkEndChild(text);
		mesh_element->LinkEndChild(name_element);

		TiXmlElement *filename_element= new TiXmlElement("filename");
		text= new TiXmlText(Utility::GetFilename(mesh->GetFilepath()).c_str());
		filename_element->LinkEndChild(text);
		mesh_element->LinkEndChild(filename_element);

		TiXmlElement *element_element= new TiXmlElement("element");
		text= new TiXmlText(mesh->GetElementPath()[0].c_str());
		element_element->LinkEndChild(text);
		mesh_element->LinkEndChild(element_element);

		entity_element->LinkEndChild(mesh_element);
	}

	xml_element->LinkEndChild(entity_element);

	return xml_element;
}
