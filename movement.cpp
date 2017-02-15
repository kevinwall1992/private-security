#include "Movement.h"
#include "Tile.h"
#include "Actor.h"

Move::Move(Tile *source_, Tile *destination_, MoveType::Enum type_)
{
	source= source_;
	destination= destination_;
	type= type_;
}

MoveType::Enum Move::GetType()
{
	return type;
}

Tile * Move::GetSource()
{
	return source;
}

Tile * Move::GetDestination()
{
	return destination;
}


AbsoluteMove::AbsoluteMove(Tile *source, Tile *destination, MoveType::Enum type_)
	: Move(source, destination, type_)
{
	
}

float AbsoluteMove::GetCost()
{
	return 1;
}

RelativeMove::RelativeMove(Tile *source, Direction::Enum direction_, MoveType::Enum type_)
	: Move(source, Direction::GetDestination(source, direction_), type_)
{
	direction= direction_;
}


Direction::Enum RelativeMove::GetDirection()
{
	return direction;
}

float RelativeMove::GetCost()
{
	return Direction::GetLength(direction);
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

vector<Move*> Mover::GetMoves(Tile *source)
{
	vector<Move *> moves;

	vector<Move *> potential_moves= GetPotentialMoves(source);
	vector<Move *> tile_enabled_moves= source->GetPotentialMoves();
	potential_moves.insert(potential_moves.end(), tile_enabled_moves.begin(), tile_enabled_moves.end());

	for(auto move : potential_moves)
		if(!DoesBlock(move) && !source->DoesBlock(move) && !move->GetDestination()->DoesBlock(move))
			moves.push_back(move);

	return moves;
}
