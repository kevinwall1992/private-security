#include "Object.h"
#include "Space.h"
#include "GameSystem.h"


string Object::GetXMLElementName()
{
	return "object";
}

void Object::LoadXML(TiXmlElement *xml_element)
{
	Entity::LoadXML(xml_element);

	SetRotation(std::stof(xml_element->FirstChildElement("rotation")->GetText()));
}

Object::Object()
{
}

Tile * Object::GetTile()
{
	return System::game.space.GetTile(GetPosition());
}

void Object::Step(Chronons chronons)
{
	HasPose::Step(chronons);
	Physical::Step(chronons);
}

Vec3f Object::GetPosition()
{
	return Physical::GetPosition();
}

float Object::GetRotation()
{
	return Physical::GetRotation();
}

void Object::SetPosition(Vec3f position)
{
	Physical::SetPosition(position);
}

void Object::SetRotation(float rotation)
{
	Physical::SetRotation(rotation);
}

TiXmlElement * Object::EncodeXML()
{
	TiXmlElement *xml_element= Entity::EncodeXML();

	TiXmlElement *xml_rotation= new TiXmlElement("rotation");
	TiXmlText *xml_rotation_text= new TiXmlText(std::to_string(GetRotation()).c_str());
	xml_rotation->LinkEndChild(xml_rotation_text);
	xml_element->LinkEndChild(xml_rotation);

	return xml_element;
}

Object * Object::DecodeXML(TiXmlElement *xml_element)
{
	Object *object_;

	string class_string= xml_element->FirstChildElement("class")->GetText();

	if(class_string== "Actor")
		object_= new Actor();
	else if(class_string== "Item")
		object_= new Item();
	else if(class_string== "Gun")
		object_= new Gun();
	else if(class_string== "Furniture")
		object_= new Furniture();
	else
		return nullptr;

	object_->LoadXML(xml_element);

	return object_;
}
