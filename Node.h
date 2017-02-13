#ifndef PS_NODE
#define PS_NODE


#include "Common.h"
#include "Actor.h"
#include "Tile.h"
#include "Edge.h"

class Node
{
	Actor *actor;
	Tile *tile;

	Node *previous= nullptr;

public:
	Node(Actor *actor, Tile *tile);

	bool operator==(Node *other);

	Tile * GetTile();

	vector <Edge *> GetEdges();

	float GetPathCost();
	float GetHeuristicCost(Node *target);

	void SetPrevious(Node *previous);
	vector<Node *> GetPath();
};

#endif