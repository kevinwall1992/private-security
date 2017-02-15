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
	Tile *source;
	Tile *destination;

	MoveType::Enum type;

public:
	Move(Tile *source, Tile *destination, MoveType::Enum type);

	MoveType::Enum GetType();

	virtual Tile * GetSource();
	virtual Tile * GetDestination();

	//this is just a stopgap
	virtual float GetCost()= 0;
};


class AbsoluteMove : public Move
{
public:
	AbsoluteMove(Tile *source, Tile *destination, MoveType::Enum type);

	float GetCost();
};


class RelativeMove : public Move
{
	Direction::Enum direction;

public:
	RelativeMove(Tile *source, Direction::Enum direction, MoveType::Enum type);

	Direction::Enum GetDirection();

	float GetCost();
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
public:
	virtual bool DoesBlock(Move *move);
};


class MovementEnabler
{
public:
	virtual vector<Move *> GetPotentialMoves(Tile *source)= 0;
};


class Mover : public MovementEnabler, public MovementBlocker
{
public:
	vector<Move *> GetMoves(Tile *source);

	virtual void PerformMove(Move *move)= 0;
};

#endif