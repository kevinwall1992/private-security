#include "Movement.h"
#include "Tile.h"
#include "Actor.h"

Move::Move(MoveType::Enum type_)
{
	type= type_;
}

MoveType::Enum Move::GetType()
{
	return type;
}


AbsoluteMove::AbsoluteMove(Tile *destination_, MoveType::Enum type_)
	: Move(type_)
{
	destination= destination_;
}

Tile * AbsoluteMove::GetDestination()
{
	return destination;
}


RelativeMove::RelativeMove(Tile *source, Direction::Enum direction_, MoveType::Enum type_)
	: Move(type_)
{
	destination= Direction::GetDestination(source, direction_);
	direction= direction_;
}


Direction::Enum RelativeMove::GetDirection()
{
	return direction;
}


Tile * RelativeMove::GetDestination()
{
	return destination;
}

void DirectionBlocker::AddBlockedDirection(Direction::Enum direction)
{
	blocked_directions.push_back(direction);
}

bool DirectionBlocker::DoesBlock(Move *move)
{
	RelativeMove *relative_move= dynamic_cast<RelativeMove *>(move);
	if(relative_move!= nullptr)
	{
		for(unsigned int i= 0; i< blocked_directions.size(); i++)
		{
			if(blocked_directions[i]== relative_move->GetDirection())
				return true;
		}
	}

	return false;
}

bool AllBlocker::DoesBlock(Move * move)
{
	return true;
}
