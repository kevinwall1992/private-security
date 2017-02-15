#ifndef PS_DIRECTION
#define PS_DIRECTION


class Tile;

struct Direction 
{
	enum Enum
	{
		Up, Down,
		East, Northeast, North, Northwest, West, Southwest, South, Southeast,
		UpEast, UpNortheast, UpNorth, UpNorthwest, UpWest, UpSouthwest, UpSouth, UpSoutheast,
		DownEast, DownNortheast, DownNorth, DownNorthwest, DownWest, DownSouthwest, DownSouth, DownSoutheast
	};

	static Direction::Enum GetDirection(Tile *source, Tile *destination);
	static Direction::Enum GetOpposite(Direction::Enum direction);
	static Tile * GetDestination(Tile *source, Direction::Enum direction);
	static float GetLength(Direction::Enum direction);
};

#endif



