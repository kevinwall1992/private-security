#include "Edge.h"
#include "Node.h"

Edge::Edge(Node *destination_, float cost_)
{
	destination= destination_;
	cost= cost_;
}

float Edge::GetCost()
{
	return cost;
}

Node * Edge::GetDestination()
{
	return destination;
}
