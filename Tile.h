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
	//temporary
	Vec3i position= Vec3i(-1, -1, -1);

	vector<Furniture *> furnitures;

	Actor *actor= nullptr;
	vector<Item *> items;

public:
	void PutFurniture(Furniture *furniture);
	bool PutActor(Actor *actor);
	void PutItem(Item *item);
	
	Actor * RemoveActor();
	Item * RemoveItem(Item *item= nullptr);

	vector<Move *> GetPotentialMoves(Tile *source= nullptr);
	bool DoesBlock(Move *move);

	bool Contains(Object *object);

	Vec3i GetPosition();

	virtual vector<Perceptible *> GetPerceptibles();
};

#endif
