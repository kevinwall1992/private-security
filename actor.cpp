#include "Actor.h"
#include "Pathing.h"
#include "Space.h"

Actor::Actor()
{
}

void Actor::PerformMove(Move *move)
{
	move->GetDestination()->PutActor(this->GetTile()->RemoveActor());
}

vector<Move *> Actor::GetMoves()
{
	vector<Move *> moves;
	moves.push_back(new RelativeMove(GetTile(), Direction::East, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::Northeast, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::North, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::Northwest, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::West, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::Southwest, MoveType::Walk));
	moves.push_back(new RelativeMove(GetTile(), Direction::Southeast, MoveType::Walk));

	return moves;
}

bool Actor::DoesBlock(Move *move)
{
	return false;
}

Entity::Pose Actor::GetPose()
{
	return Entity::Pose(Entity::State::Stand);
}

string Actor::GetEntityDataFilename()
{
	return "actor.xml";
}

string Actor::GetEntityDataFolderName()
{
	return "actors";
}
