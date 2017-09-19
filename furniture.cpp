#include "Furniture.h"


void Furniture::LoadXML(TiXmlElement *xml_element)
{
	Object::LoadXML(xml_element);

	TiXmlElement *xml_blocked_directions_list= xml_element->FirstChildElement("blocking");
	TiXmlElement *xml_child_element= xml_blocked_directions_list->FirstChildElement();
	while(xml_child_element!= nullptr)
	{
		string direction_string= xml_child_element->GetText();

		if(direction_string== "all")
		{
			for(int i= 0; i< Direction::DirectionCount; i++)
				AddBlockedDirection((Direction::Enum)i);
		}
		else
			AddBlockedDirection(Direction::GetDirection(direction_string));

		xml_child_element= xml_child_element->NextSiblingElement();
	}
}

Furniture::Furniture()
{

}

vector<Move*> Furniture::GetPotentialMoves(Tile *source)
{
	return vector<Move*>();
}

string Furniture::GetEntityDataFilename()
{
	return "furniture.xml";
}

string Furniture::GetEntityDataFolderName()
{
	return "furniture";
}

TiXmlElement * Furniture::EncodeXML()
{
	TiXmlElement *xml_element= Object::EncodeXML();

	xml_element->LinkEndChild(Utility::XML::MakeElementWithText("class", "Furniture"));

	TiXmlElement *xml_blocked_directions_list= new TiXmlElement("blocking");

	bool all_directions_blocked= true;
	for(int i= 0; i< Direction::DirectionCount; i++)
		if(!IsDirectionBlocked((Direction::Enum)i))
			all_directions_blocked= false;

	if(all_directions_blocked)
	{
		TiXmlElement *xml_direction= new TiXmlElement("direction");
		TiXmlText *xml_text= new TiXmlText("all");
		xml_direction->LinkEndChild(xml_text);
		xml_blocked_directions_list->LinkEndChild(xml_direction);
	}
	else
	{
		for(int i= 0; i< Direction::DirectionCount; i++)
		{
			if(IsDirectionBlocked((Direction::Enum)i))
			{
				TiXmlElement *xml_direction= new TiXmlElement("direction");
				TiXmlText *xml_text= new TiXmlText(Direction::GetDirectionString((Direction::Enum)i).c_str());
				xml_direction->LinkEndChild(xml_text);
				xml_blocked_directions_list->LinkEndChild(xml_direction);
			}
		}
	}

	xml_element->LinkEndChild(xml_blocked_directions_list);

	return xml_element;
}
