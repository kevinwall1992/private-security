#ifndef PS_SPACE
#define PS_SPACE

#include "Tile.h"
#include "Perceptible.h"


struct Space: public PerceptibleContainer
{
	static Space *space;

	Tile tiles[10][10][1];

	Space();

	Vec3i GetTilePosition(Tile *tile);
	Vec3i GetObjectPosition(Object *object);

	virtual vector<Perceptible *> GetPerceptibles();
};

#endif