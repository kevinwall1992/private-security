#ifndef PS_TILE
#define PS_TILE

#include "Object.h"
#include "Movement.h"
#include "Actor.h"
#include "Item.h"
#include "Furniture.h"
#include "Math.h"
#include "Chronon.h"

class Space;


class Tile : public MovementEnabler, public MovementBlocker, public PerceptibleContainer, public HasPosition, public Chronal
{
	Space *space;

	vector<Furniture *> furnitures;

	Actor *actor= nullptr;
	vector<Item *> items;

	void SetSpace(Space *space);
	friend class Space;

public:
	

	void PutFurniture(Furniture *furniture);
	bool PutActor(Actor *actor);
	void PutItem(Item *item);
	
	Actor * RemoveActor();
	Item * RemoveItem(Item *item= nullptr);
	Furniture * RemoveFurniture(Furniture *furniture);

	vector<Furniture *> GetFurnitures();
	bool HasActor();
	Actor * GetActor();
	vector<Item *> GetItems();

	vector<Move *> GetPotentialMoves(Tile *source= nullptr);
	bool DoesBlock(Move *move);

	bool Contains(Object *object);

	Vec3f GetPosition();

	virtual vector<Perceptible *> GetPerceptibles();

	void Step(Chronons chronons);
};

#endif
