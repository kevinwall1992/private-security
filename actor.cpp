#include "Actor.h"
#include "Pathing.h"
#include "Space.h"
#include "Action.h"


Actor::Actor()
{
	inventory.push_back(new Gun());
}

Vec3f Actor::GetPosition()
{
	return Physical::GetPosition();
}

float Actor::GetRotation()
{
	return rotation;
}

vector<Item*> Actor::GetInventory()
{
	return inventory;
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

string Actor::GetEntityAnimationName()
{
	if(action== nullptr)
		return Entity::GetEntityAnimationName();

	return action->GetName();
}

float Actor::GetEntityAnimationMoment()
{
	if(action== nullptr)
		return Entity::GetEntityAnimationMoment();

	return action->GetDimensionalProgress();
}

void Actor::SetAction(Action *action_)
{
	action= action_;
}

Action * Actor::GetAction()
{
	return action;
}

void Actor::Step(Chronons chronons)
{
	Object::Step(chronons);
	Physical::Step(chronons);
}
