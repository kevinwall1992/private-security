#include "Tile.h"
#include "Space.h"

void Tile::PutFurniture(Furniture *furniture)
{
	furnitures.push_back(furniture);
}

bool Tile::PutActor(Actor *actor_)
{
	if(actor!= nullptr)
		return false;

	actor= actor_;
	return true;
}

void Tile::PutItem(Item *item)
{
	items.push_back(item);
}

Actor * Tile::RemoveActor()
{
	Actor *actor_= actor;
	actor= nullptr;

	return actor;
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

vector<Move *> Tile::GetMoves()
{
	vector<Move *> moves;

	for(unsigned int i= 0; i< furnitures.size(); i++)
	{
		vector<Move *> furniture_enabled_moves= furnitures[i]->GetMoves();

		for(unsigned int i= 0; i< furniture_enabled_moves.size(); i++)
			moves.push_back(furniture_enabled_moves[i]);
	}

	return moves;
}

bool Tile::DoesBlock(Move *move)
{
	if(actor!= nullptr && move->GetDestination()== this)
		return true;

	for(unsigned int i= 0; i< furnitures.size(); i++)
		if(furnitures[i]->DoesBlock(move))
			return true;

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
	return Space::space->GetTilePosition(this);
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

/*void Tile::Draw()
{
	for(unsigned int i= 0; i< furnitures.size(); i++)
		furnitures[i]->Draw();

	for(unsigned int i= 0; i< items.size(); i++)
		items[i]->Draw();

	if(actor!= nullptr)
		actor->Draw();
}*/
