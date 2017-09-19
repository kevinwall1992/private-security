#include "Item.h"


void Item::LoadXML(TiXmlElement *xml_element)
{
	Object::LoadXML(xml_element);
}

Item::Item()
{
}

string Item::GetEntityDataFilename()
{
	return "item.xml";
}

string Item::GetEntityDataFolderName()
{
	return "items";
}

TiXmlElement * Item::EncodeXML()
{
	TiXmlElement *xml_element= Object::EncodeXML();

	xml_element->LinkEndChild(Utility::XML::MakeElementWithText("class", "Item"));

	return xml_element;
}


void Gun::LoadXML(TiXmlElement *xml_element)
{
	Item::LoadXML(xml_element);
}

Gun::Gun()
{
}

string Gun::GetEntityDataFilename()
{
	return "gun.xml";
}

TiXmlElement * Gun::EncodeXML()
{
	TiXmlElement *xml_element= Item::EncodeXML();

	Utility::XML::AddOrReplaceChildElement(xml_element, Utility::XML::MakeElementWithText("class", "Gun"));

	return xml_element;
}