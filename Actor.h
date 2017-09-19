#ifndef PS_ACTOR
#define PS_ACTOR

#include "Object.h"
#include "Tile.h"
#include "Item.h"
#include "Movement.h"
#include "Perceptible.h"
#include "Physical.h"


class Action;

typedef vector<Item *> Inventory;

class Actor : public Object, public Mover
{
	Inventory inventory;

	Action *action= nullptr;

protected:
	virtual void LoadXML(TiXmlElement *xml_element);

public:
	Actor();

	void GiveItem(Item *item);
	void TakeItem(Item *item);
	Inventory GetInventory();

	void PerformMove(Move *move);

	vector<Move *> GetPotentialMoves(Tile *source);
	bool DoesBlock(Move *move);

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();
	virtual string GetEntityAnimationName();
	virtual float GetEntityAnimationMoment();

	//This is a temporary implementation.
	//Setting an action doesn't do anything, it just makes it accessible to
	//those who own a reference to Actor
	void SetAction(Action *action);
	Action * GetAction();

	virtual TiXmlElement * EncodeXML();
};

#endif