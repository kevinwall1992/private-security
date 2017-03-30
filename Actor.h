#ifndef PS_ACTOR
#define PS_ACTOR

#include "Object.h"
#include "Tile.h"
#include "Item.h"
#include "Movement.h"
#include "Perceptible.h"
#include "Physical.h"


typedef vector<Item *> Inventory;

class Actor : public Object, public Mover, public Physical
{
	Inventory inventory;
	Item *item = new Gun();

public:
	Actor();

	Vec3f GetPosition();

	
	Inventory GetInventory();

	void PerformMove(Move *move);

	vector<Move *> GetPotentialMoves(Tile *source);
	bool DoesBlock(Move *move);

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();

	virtual void Step(Chronons chronons);
};

#endif