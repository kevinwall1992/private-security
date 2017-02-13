#ifndef PS_TILE
#define PS_TILE


#include "Object.h"
#include "Movement.h"
#include "Actor.h"
#include "Item.h"
#include "Furniture.h"
#include "Math.h"

class Tile : public MovementEnabler, public MovementBlocker, public PerceptibleContainer
{
	vector<Furniture *> furnitures;

	Actor *actor= nullptr;
	vector<Item *> items;

public:
	void PutFurniture(Furniture *furniture);
	bool PutActor(Actor *actor);
	void PutItem(Item *item);
	
	Actor * RemoveActor();
	Item * RemoveItem(Item *item= nullptr);

	vector<Move *> GetMoves();
	bool DoesBlock(Move *move);

	bool Contains(Object *object);

	Vec3f GetPosition();

	virtual vector<Perceptible *> GetPerceptibles();
};

#endif
