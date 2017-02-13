#ifndef PS_ACTOR
#define PS_ACTOR


#include "Object.h"
#include "Tile.h"
#include "Item.h"
#include "Movement.h"
#include "Perceptible.h"

class Actor : public Object, public MovementEnabler, public MovementBlocker
{
	Item *item = new Gun();

public:
	Actor();

	void PerformMove(Move *move);

	vector<Move *> GetMoves();
	bool DoesBlock(Move *move);

	virtual Pose GetPose();
	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();
};

#endif