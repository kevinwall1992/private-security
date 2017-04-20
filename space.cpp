#include "Space.h"
#include "GameSystem.h"


Vec3i Space::IndexToPosition(int index)
{
	int column= index% column_count;
	int row= (index% (column_count* row_count))/ column_count;
	int level= index/ (column_count* row_count);

	return Vec3i(column, level, row);
}

int Space::GetTileCount()
{
	return column_count* row_count* level_count;
}

Space::Space(int column_count_, int row_count_, int level_count_)
{
	column_count= column_count_;
	row_count= row_count_;
	level_count= level_count_;

	tiles= new Tile[column_count* row_count* level_count];
}

Tile * Space::GetTile(int column, int row, int level)
{
	if((column< 0 || column>= column_count) || (row< 0 || row>= row_count) || (level< 0 || level>= level_count))
		return nullptr;

	return &tiles[level* column_count* row_count+ row* column_count+ column];
}

Tile * Space::GetTile(Vec3i position)
{
	return GetTile(position.x, position.z, position.y);
}

Vec3i Space::GetPosition(Tile *tile)
{
	for(int i= 0; i< GetTileCount(); i++)
		if(&tiles[i]== tile)
			return IndexToPosition(i);

	return Vec3i(-1, -1, -1);
}

Vec3i Space::GetPosition(Object *object)
{
	for(int i= 0; i< GetTileCount(); i++)
		if(tiles[i].Contains(object))
			return IndexToPosition(i);

	return Vec3i(-1, -1, -1);
}

Vec3i Space::GetSize()
{
	return Vec3i(column_count, level_count, row_count);
}

vector<Perceptible*> Space::GetPerceptibles()
{
	vector<Perceptible *> perceptibles;

	for(int i= 0; i< GetTileCount(); i++)
		perceptibles.push_back(&tiles[i]);

	return perceptibles;
}

void Space::Step(Chronons chronons)
{
	for(int i= 0; i< GetTileCount(); i++)
		tiles[i].Step(chronons);
}
