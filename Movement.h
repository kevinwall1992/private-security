#ifndef PS_MOVEMENT
#define PS_MOVEMENT


#include "Common.h"
#include "Direction.h"

class Tile;
class Actor;


struct MoveType
{
	enum Enum
	{
		Walk, Ladder, Teleport
	};
};

class Move
{
	MoveType::Enum type;

public:
	Move(MoveType::Enum type);

	MoveType::Enum GetType();

	virtual Tile * GetDestination()= 0;
};

class AbsoluteMove : public Move
{
	Tile *destination;

public:
	AbsoluteMove(Tile *destination, MoveType::Enum type);

	Tile * GetDestination();
};

class RelativeMove : public Move
{
	Tile *destination;
	Direction::Enum direction;

public:
	RelativeMove(Tile *source, Direction::Enum direction, MoveType::Enum type);

	Direction::Enum GetDirection();

	Tile * GetDestination();

};

class MovementBlocker
{
public:
	virtual bool DoesBlock(Move *move)= 0;
};

class DirectionBlocker : public MovementBlocker
{
	vector<Direction::Enum> blocked_directions;

public:

	void AddBlockedDirection(Direction::Enum direction);

	virtual bool DoesBlock(Move *move);
};

class AllBlocker : public MovementBlocker
{
	virtual bool DoesBlock(Move *move);
};

class MovementEnabler
{
public:
	virtual vector<Move *> GetMoves()= 0;
};

#endif