#ifndef PS_EDGE
#define PS_EDGE

class Node;

class Edge
{
	Node *destination;
	float cost;

public:
	Edge(Node *destination, float cost);

	float GetCost();
	Node * GetDestination();
};

#endif