#ifndef PS_FURNITURE
#define PS_FURNITURE


#include "Object.h"
#include "Movement.h"

class Furniture : public Object, public MovementEnabler, public DirectionBlocker
{
public:
	Furniture();

	virtual vector<Move *> GetPotentialMoves(Tile *source);
	//virtual bool DoesBlock(Move *move);

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();
};

class ThickWall : public Furniture, public AllBlocker
{
public:
	ThickWall();

	virtual bool DoesBlock(Move *move);

	virtual string GetEntityDataFilename();
};

class InvisibleThickWall : public ThickWall
{
public:
	InvisibleThickWall();

	virtual string GetEntityDataFilename();
};

class Museum : public Furniture
{
public:
	Museum();

	virtual string GetEntityDataFilename();
};

#endif