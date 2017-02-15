#ifndef PS_ACTOR
#define PS_ACTOR


#include "Object.h"
#include "Tile.h"
#include "Item.h"
#include "Movement.h"
#include "Perceptible.h"

class Actor : public Object, public Mover
{
	Item *item = new Gun();

public:
	Actor();

	void PerformMove(Move *move);

	vector<Move *> GetPotentialMoves(Tile *source);
	bool DoesBlock(Move *move);

	virtual Pose GetPose();
	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();
};

#endif