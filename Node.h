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

	Edge *in= nullptr;

public:
	//Actor should probably be replaced with some interface that just means
	//"has movement capability" because currently this is misleading; 
	//You don't need an actor, you just need an oracle that tells you if a move
	//is possible.
	Node(Actor *actor, Tile *tile);

	bool operator==(const Node &other);

	Tile * GetTile();

	vector <Edge *> GetEdges();

	float GetPathCost();
	float GetHeuristicCost(Node *target);

	void SetInEdge(Edge *in);
	vector<Edge *> GetPath();
};

#endif