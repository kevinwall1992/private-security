#ifndef PS_EDGE
#define PS_EDGE

#include "Movement.h"

class Node;

class Edge
{
	Node *source;
	Node *destination;
	
	Move *move;

public:
	Edge(Node *source, Node *destination, Move *move);

	float GetCost();

	Node * GetSource();
	Node * GetDestination();

	Move * GetMove();
};

#endif