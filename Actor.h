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

class Actor : public Object, public Mover, public Physical
{
	Inventory inventory;
	Item *item = new Gun();

	Action *action= nullptr;

public:
	Actor();

	Vec3f GetPosition();
	float GetRotation();

	
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

	virtual void Step(Chronons chronons);
};

#endif