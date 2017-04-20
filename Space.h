#ifndef PS_SPACE
#define PS_SPACE

#include "Tile.h"
#include "Perceptible.h"
#include "Property.h"
#include "Chronon.h"


class Space: public PerceptibleContainer, public Chronal
{
	Tile *tiles;
	int column_count, row_count, level_count;

	Vec3i IndexToPosition(int index);
	int GetTileCount();

public:
	Space(int column_count, int row_count, int level_count);

	Tile * GetTile(int column, int row, int level);
	Tile * GetTile(Vec3i position);

	Vec3i GetPosition(Tile *tile);
	Vec3i GetPosition(Object *object);

	Vec3i GetSize();

	virtual vector<Perceptible *> GetPerceptibles();

	void Step(Chronons chronons);
};

#endif
