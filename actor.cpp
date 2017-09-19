#include "Actor.h"
#include "Pathing.h"
#include "Space.h"
#include "Action.h"


void Actor::LoadXML(TiXmlElement *xml_element)
{
	Object::LoadXML(xml_element);

	TiXmlElement *xml_inventory= xml_element->FirstChildElement("inventory");
	TiXmlElement *xml_item= xml_inventory->FirstChildElement();
	while(xml_item!= nullptr)
	{
		inventory.push_back(Encodable::DecodeXML<Item>(xml_item));
		xml_item= xml_item->NextSiblingElement();
	}
}

Actor::Actor()
{
	
}

void Actor::GiveItem(Item *item)
{
	inventory.push_back(item);
}

void Actor::TakeItem(Item *item)
{
	for(unsigned int i= 0; i< inventory.size(); i++)
		if(inventory[i]== item)
			inventory.erase(inventory.begin()+ i--);
}

vector<Item*> Actor::GetInventory()
{
	return inventory;
}

void Actor::PerformMove(Move *move)
{
	move->GetDestination()->PutActor(this->GetTile()->RemoveActor());
}

vector<Move *> Actor::GetPotentialMoves(Tile *source)
{
	vector<Move *> moves;
	moves.push_back(new RelativeMove(source, Direction::East, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Northeast, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::North, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Northwest, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::West, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Southwest, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::South, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Southeast, MoveType::Walk));

	for(unsigned int i= 0; i< moves.size(); i++)
		if(moves[i]->GetDestination()== nullptr)
			moves.erase(moves.begin()+ i--);

	return moves;
}

bool Actor::DoesBlock(Move *move)
{
	return false;
}

string Actor::GetEntityDataFilename()
{
	return "actor.xml";
}

string Actor::GetEntityDataFolderName()
{
	return "actors";
}

string Actor::GetEntityAnimationName()
{
	if(action== nullptr)
		return Entity::GetEntityAnimationName();

	return action->GetName();
}

float Actor::GetEntityAnimationMoment()
{
	if(action== nullptr)
		return Entity::GetEntityAnimationMoment();

	return action->GetDimensionalProgress();
}

void Actor::SetAction(Action *action_)
{
	action= action_;
}

Action * Actor::GetAction()
{
	return action;
}

TiXmlElement * Actor::EncodeXML()
{
	TiXmlElement *xml_element= Object::EncodeXML();

	xml_element->LinkEndChild(Utility::XML::MakeElementWithText("class", "Actor"));

	TiXmlElement *xml_inventory= new TiXmlElement("inventory");
	for(Item *item : inventory)
		xml_inventory->LinkEndChild(item->EncodeXML());
	xml_element->InsertEndChild(*xml_inventory);

	return xml_element;
}