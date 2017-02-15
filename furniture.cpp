#include "Furniture.h"

Furniture::Furniture()
{

}

vector<Move*> Furniture::GetPotentialMoves(Tile *source)
{
	return vector<Move*>();
}

string Furniture::GetEntityDataFilename()
{
	return "furniture.xml";
}

string Furniture::GetEntityDataFolderName()
{
	return "furniture";
}

ThickWall::ThickWall()
{

}

bool ThickWall::DoesBlock(Move *move)
{
	return AllBlocker::DoesBlock(move);
}

string ThickWall::GetEntityDataFilename()
{
	return "wall.xml";
}
