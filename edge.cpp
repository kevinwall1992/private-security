#include "Edge.h"
#include "Node.h"

Edge::Edge(Node *source_, Node *destination_, Move *move_)
{
	source= source_;
	destination= destination_;
	move= move_;

	destination->SetInEdge(this);
}

float Edge::GetCost()
{
	return move->GetCost();
}

Node * Edge::GetSource()
{
	return source;
}

Node * Edge::GetDestination()
{
	return destination;
}

Move * Edge::GetMove()
{
	return move;
}
