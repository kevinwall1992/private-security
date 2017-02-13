#include "Furniture.h"

Furniture::Furniture()
{

}

vector<Move*> Furniture::GetMoves()
{
	return vector<Move*>();
}

bool Furniture::DoesBlock(Move * move)
{
	return false;
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

string ThickWall::GetEntityDataFilename()
{
	return "wall.xml";
}
