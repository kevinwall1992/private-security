#include "Actor.h"
#include "Pathing.h"
#include "Space.h"

Actor::Actor()
{
}

Vec3f Actor::GetPosition()
{
	return Physical::GetPosition();
}

void Actor::PerformMove(Move *move)
{
	move->GetDestination()->PutActor(this->GetTile()->RemoveActor());
}

vector<Move *> Actor::GetPotentialMoves(Tile *source)
{
	vector<Move *> moves;
	moves.push_back(new RelativeMove(source, Direction::East, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Northeast, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::North, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Northwest, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::West, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Southwest, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::South, MoveType::Walk));
	moves.push_back(new RelativeMove(source, Direction::Southeast, MoveType::Walk));

	for(unsigned int i= 0; i< moves.size(); i++)
		if(moves[i]->GetDestination()== nullptr)
			moves.erase(moves.begin()+ i--);

	return moves;
}

bool Actor::DoesBlock(Move *move)
{
	return false;
}

string Actor::GetEntityDataFilename()
{
	return "actor.xml";
}

string Actor::GetEntityDataFolderName()
{
	return "actors";
}

void Actor::Step(Chronons chronons)
{
	Object::Step(chronons);
	Physical::Step(chronons);
}
