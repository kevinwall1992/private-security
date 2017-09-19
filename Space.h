#ifndef PS_SPACE
#define PS_SPACE

#include "Tile.h"
#include "Perceptible.h"
#include "Property.h"
#include "Chronon.h"
#include "Savable.h"

#include <string>

using std::string;


class Space: public PerceptibleContainer, public Chronal, public Savable
{
	Tile *tiles= nullptr;
	int column_count, row_count, level_count;

	Vec3i IndexToPosition(int index);
	int GetTileCount();

	void DeleteContents();
	void Resize(int column_count, int row_count, int level_count);

public:
	Space(int column_count, int row_count, int level_count);
	Space();

	Space(string filepath);

	Tile * GetTile(int column, int row, int level);
	Tile * GetTile(Vec3i position);

	Vec3i GetPosition(Tile *tile);
	Vec3i GetPosition(Object *object);

	Vec3i GetSize();

	virtual vector<Perceptible *> GetPerceptibles();

	void Step(Chronons chronons);

	void Save(string filepath);
	void Load(string filepath);
};

#endif
