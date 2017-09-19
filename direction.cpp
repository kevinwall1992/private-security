#include "Direction.h"
#include "Space.h"
#include "GameSystem.h"

Direction::Enum Direction::GetDirection(Tile *source, Tile *destination)
{
	return Direction::Enum();
}

Direction::Enum Direction::GetOpposite(Direction::Enum direction)
{
	switch(direction)
	{
	case Up: return Down;
	case Down: return Up;
	case East: return West;
	case Northeast: return Southwest;
	case North: return South;
	case Northwest: return Southeast;
	case West: return East;
	case Southwest: return Northeast;
	case South: return North;
	case Southeast: return Northwest;
	case UpEast: return DownWest;
	case UpNortheast: return DownSouthwest;
	case UpNorth: return DownSouth;
	case UpNorthwest: return DownSoutheast;
	case UpWest: return DownEast;
	case UpSouthwest: return DownNortheast;
	case UpSouth: return DownNorth;
	case UpSoutheast: return DownNorthwest;
	case DownEast: return UpWest;
	case DownNortheast: return UpSouthwest;
	case DownNorth: return UpSouth;
	case DownNorthwest: return UpSoutheast;
	case DownWest: return UpEast;
	case DownSouthwest: return UpNortheast;
	case DownSouth: return UpNorth;
	case DownSoutheast: return UpNorthwest;
	default: return (Direction::Enum)0;
	}
}

Tile * Direction::GetDestination(Tile *source, Direction::Enum direction)
{
	Vec3i source_position= System::game.space.GetPosition(source);
	Vec3i offset(0, 0, 0);

	switch(direction)
	{
	case Down: 
	case DownEast: 
	case DownNortheast: 
	case DownNorth: 
	case DownNorthwest: 
	case DownWest: 
	case DownSouthwest: 
	case DownSouth: 
	case DownSoutheast: 
		offset+= Vec3i(0, -1, 0);
		break;

	case Up: 
	case UpEast: 
	case UpNortheast: 
	case UpNorth: 
	case UpNorthwest: 
	case UpWest: 
	case UpSouthwest: 
	case UpSouth: 
	case UpSoutheast:
		offset+= Vec3i(0, 1, 0);
		break;

	default: break;
	}

	switch(direction)
	{
	case Northwest: 
	case West: 
	case Southwest: 
	case UpNorthwest: 
	case UpWest: 
	case UpSouthwest: 
	case DownNorthwest: 
	case DownWest: 
	case DownSouthwest: 
		offset+= Vec3i(-1, 0, 0);
		break;

	case East: 
	case Northeast: 
	case Southeast: 
	case UpEast: 
	case UpNortheast: 
	case UpSoutheast: 
	case DownEast: 
	case DownNortheast:
	case DownSoutheast: 
		offset+= Vec3i(1, 0, 0);
		break;

	default: break;
	}

	switch(direction)
	{
	case Southwest: 
	case South: 
	case Southeast: 
	case UpSouthwest: 
	case UpSouth: 
	case UpSoutheast: 
	case DownSouthwest: 
	case DownSouth: 
	case DownSoutheast: 
		offset+= Vec3i(0, 0, -1);
		break;

	case Northeast: 
	case North: 
	case Northwest: 
	case UpNortheast: 
	case UpNorth: 
	case UpNorthwest: 
	case DownNortheast: 
	case DownNorth: 
	case DownNorthwest: 
		offset+= Vec3i(0, 0, 1);
		break;

	default: break;
	}

	return System::game.space.GetTile(source_position+ offset);
}

float Direction::GetLength(Direction::Enum direction)
{
	switch(direction)
	{
	case Up: 
	case Down: 
	case East: 
	case North: 
	case West: 
	case South:
		return 1.0f;

	case Northeast: 
	case Northwest: 
	case Southwest: 
	case Southeast: 
	case UpEast: 
	case UpNorth: 
	case UpWest: 
	case UpSouth: 
	case DownEast: 
	case DownNorth: 
	case DownWest: 
	case DownSouth: 
		return (float)sqrt(2);
		
	case UpNortheast: 
	case UpNorthwest: 
	case UpSouthwest: 
	case UpSoutheast: 
	case DownNortheast: 
	case DownNorthwest: 
	case DownSouthwest: 
	case DownSoutheast: 
		return (float)sqrt(3);

	default: return 0.0f;
	}
}

string Direction::GetDirectionString(Direction::Enum direction)
{
	string direction_string;

	switch(direction)
	{
	case Direction::Up: return "Up"; 
	case Direction::Down: return "Down";
	case Direction::East: return "East"; 
	case Direction::Northeast: return "Northeast"; 
	case Direction::North: return "North"; 
	case Direction::Northwest: return "Northwest"; 
	case Direction::West: return "West"; 
	case Direction::Southwest: return "Southwest"; 
	case Direction::South: return "South"; 
	case Direction::Southeast: return "Southeast"; 
	case Direction::UpEast: return "UpEast"; 
	case Direction::UpNortheast: return "UpNortheast"; 
	case Direction::UpNorth: return "UpNorth"; 
	case Direction::UpNorthwest: return "UpNorthwest"; 
	case Direction::UpWest: return "UpWest"; 
	case Direction::UpSouthwest: return "UpSouthwest"; 
	case Direction::UpSouth: return "UpSouth"; 
	case Direction::UpSoutheast: return "UpSoutheast"; 
	case Direction::DownEast: return "DownEast"; 
	case Direction::DownNortheast: return "DownNortheast"; 
	case Direction::DownNorth: return "DownNorth"; 
	case Direction::DownNorthwest: return "DownNorthwest"; 
	case Direction::DownWest: return "DownWest"; 
	case Direction::DownSouthwest: return "DownSouthwest"; 
	case Direction::DownSouth: return "DownSouth"; 
	case Direction::DownSoutheast: return "DownSoutheast";
	default: 
		assert(false && "furniture.cpp::GetDirectionString(): invalid direction");
		return "";
	}
}

Direction::Enum Direction::GetDirection(string direction_string)
{
	if(direction_string== "Up") return Direction::Up;
	else if(direction_string== "Down") return Direction::Down;
	else if(direction_string== "East") return Direction::East;
	else if(direction_string== "Northeast") return Direction::Northeast; 
	else if(direction_string== "North") return Direction::North;
	else if(direction_string== "Northwest") return Direction::Northwest;
	else if(direction_string== "West") return Direction::West;
	else if(direction_string== "Southwest") return Direction::Southwest;
	else if(direction_string== "South") return Direction::South;
	else if(direction_string== "Southeast") return Direction::Southeast; 
	else if(direction_string== "UpEast") return Direction::UpEast;
	else if(direction_string== "UpNortheast") return Direction::UpNortheast;
	else if(direction_string== "UpNorth") return Direction::UpNorth;
	else if(direction_string== "UpNorthwest") return Direction::UpNorthwest;
	else if(direction_string== "UpWest") return Direction::UpWest;
	else if(direction_string== "UpSouthwest") return Direction::UpSouthwest;
	else if(direction_string== "UpSouth") return Direction::UpSouth;
	else if(direction_string== "UpSoutheast") return Direction::UpSoutheast;
	else if(direction_string== "DownEast") return Direction::DownEast;
	else if(direction_string== "DownNortheast") return Direction::DownNortheast; 
	else if(direction_string== "DownNorth") return Direction::DownNorth;
	else if(direction_string== "DownNorthwest") return Direction::DownNorthwest;
	else if(direction_string== "DownWest") return Direction::DownWest;
	else if(direction_string== "DownSouthwest") return Direction::DownSouthwest; 
	else if(direction_string== "DownSouth") return Direction::DownSouth;
	else if(direction_string== "DownSoutheast") return Direction::DownSoutheast;

	assert(false && "furniture.cpp::GetDirection(): direction string did not match.");
	return (Direction::Enum)0;
}