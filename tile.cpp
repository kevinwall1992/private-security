#include "Tile.h"
#include "Space.h"
#include "GameSystem.h"

void Tile::SetSpace(Space *space_)
{
	space= space_;
}

void Tile::PutFurniture(Furniture *furniture)
{
	furnitures.push_back(furniture);

	furniture->SetPosition(GetPosition());
}

bool Tile::PutActor(Actor *actor_)
{
	if(actor!= nullptr)
		return false;

	actor= actor_;
	actor->SetPosition(GetPosition());

	return true;
}

void Tile::PutItem(Item *item)
{
	items.push_back(item);
	
	item->SetPosition(GetPosition());
}

Actor * Tile::RemoveActor()
{
	Actor *actor_= actor;
	actor= nullptr;

	return actor_;
}

Item * Tile::RemoveItem(Item *item)
{
	if(item!= nullptr)
	{
		for(unsigned int i= 0; i< items.size(); i++)
			if(item== items[i])
				items.erase(items.begin()+ i);
	}
	else
	{
		item= items[items.size()- 1];
		items.erase(items.end());
	}

	return item;
}

Furniture * Tile::RemoveFurniture(Furniture *furniture)
{
	for(unsigned int i= 0; i< furnitures.size(); i++)
		if(furnitures[i]== furniture)
			furnitures.erase(furnitures.begin()+ i--);

	return furniture;
}

vector<Furniture*> Tile::GetFurnitures()
{
	return furnitures;
}

bool Tile::HasActor()
{
	return actor!= nullptr;
}

Actor * Tile::GetActor()
{
	return actor;
}

vector<Item*> Tile::GetItems()
{
	return items;
}

vector<Move *> Tile::GetPotentialMoves(Tile *source)
{
	//This is another reason for parameterizing Moves with source tiles later
	assert(source== nullptr && "Tile::GetPotentialMoves: source is not supposed to be used.");

	vector<Move *> moves;

	for(unsigned int i= 0; i< furnitures.size(); i++)
	{
		vector<Move *> furniture_enabled_moves= furnitures[i]->GetPotentialMoves(this);

		for(unsigned int i= 0; i< furniture_enabled_moves.size(); i++)
			moves.push_back(furniture_enabled_moves[i]);
	}

	return moves;
}

bool Tile::DoesBlock(Move *move)
{
	GetPosition();

	if(move->GetSource()== this)
	{
		for(unsigned int i= 0; i< furnitures.size(); i++)
			if(furnitures[i]->DoesBlock(move))
				return true;
	}
	else if(move->GetDestination()== this)
	{
		if(actor!= nullptr)
			return true;

		RelativeMove *relative_move= dynamic_cast<RelativeMove *>(move);
		if(relative_move!= nullptr)
			move= new RelativeMove(this, Direction::GetOpposite(relative_move->GetDirection()), move->GetType());

		bool does_block= false;
		for(unsigned int i= 0; i< furnitures.size(); i++)
			if(furnitures[i]->DoesBlock(move))
				does_block= true;

		delete move;

		return does_block;
	}

	return false;
}

bool Tile::Contains(Object *object)
{
	for(unsigned int i= 0; i< furnitures.size(); i++)
		if(furnitures[i]== object)
			return true;

	for(unsigned int i= 0; i< items.size(); i++)
		if(items[i]== object)
			return true;

	if(actor== object)
		return true;

	return false;
}

Vec3f Tile::GetPosition()
{
	return space->GetPosition(this);
}

vector<Perceptible *> Tile::GetPerceptibles()
{
	vector<Perceptible *> perceptibles;

	for(unsigned int i= 0; i< furnitures.size(); i++)
		perceptibles.push_back(furnitures[i]);

	for(unsigned int i= 0; i< items.size(); i++)
		perceptibles.push_back(items[i]);

	if(actor!= nullptr)
		perceptibles.push_back(actor);

	return perceptibles;
}

void Tile::Step(Chronons chronons)
{
	for(Item *item : items)
		item->Step(chronons);

	for(Furniture *furniture : furnitures)
		furniture->Step(chronons);

	if(actor!= nullptr)
		actor->Step(chronons);
}

/*void Tile::Draw()
{
	for(unsigned int i= 0; i< furnitures.size(); i++)
		furnitures[i]->Draw();

	for(unsigned int i= 0; i< items.size(); i++)
		items[i]->Draw();

	if(actor!= nullptr)
		actor->Draw();
}*/
