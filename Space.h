#ifndef PS_SPACE
#define PS_SPACE

#include "Tile.h"
#include "Perceptible.h"
#include "Property.h"


//Need to seriously consider whether space should
//use an xz plane with levels represented by y
//This makes positions interchangeable between logic and visualization
class Space: public PerceptibleContainer
{
	int width= 10, height= 10, depth= 3;

public:
	static Space *space;

	Tile tiles[10][10][3];

	ImmutableProperty<int> Width, Height, Depth;

	Space();

	Vec3i GetTilePosition(Tile *tile);
	Vec3i GetObjectPosition(Object *object);

	virtual vector<Perceptible *> GetPerceptibles();
};

#endif