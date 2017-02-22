#ifndef PS_SPACE
#define PS_SPACE

#include "Tile.h"
#include "Perceptible.h"
#include "Property.h"
#include "Chronon.h"


//Need to seriously consider whether space should
//use an xz plane with levels represented by y
//This makes positions interchangeable between logic and visualization

template<int width, int height, int depth>
class Space: public PerceptibleContainer, public Chronal
{
	Tile * GetFlatTileArray()
	{
		return (Tile *)tiles;
	}

	Vec3i IndexToPosition(int index)
	{
		int x= index/ (height* depth);
		int y= (index/ depth)% height;
		int z= index% depth;

		return Vec3i(x, y, z);
	}

	int GetTileCount()
	{
		return width* height* depth;
	}

public:
	Tile tiles[width][height][depth];

	const int Width, Height, Depth;

	Space()
		: Width(width), Height(height), Depth(depth)
	{

	}

	Vec3i GetTilePosition(Tile *tile)
	{	
		for(int i= 0; i< GetTileCount(); i++)
			if(&GetFlatTileArray()[i]== tile)
				return IndexToPosition(i);

		return Vec3i(-1, -1, -1);
	}

	Vec3i GetObjectPosition(Object *object)
	{
		for(int i= 0; i< GetTileCount(); i++)
			if(GetFlatTileArray()[i].Contains(object))
				return IndexToPosition(i);

		return Vec3i(-1, -1, -1);
	}

	virtual vector<Perceptible *> GetPerceptibles()
	{
		vector<Perceptible *> perceptibles;

		for(int i= 0; i< GetTileCount(); i++)
			perceptibles.push_back(&GetFlatTileArray()[i]);

		return perceptibles;
	}

	void Step(Chronons chronons)
	{
		for(int i= 0; i< GetTileCount(); i++)
			GetFlatTileArray()[i].Step(chronons);
	}
};

#endif