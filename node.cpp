#include "Node.h"

Node::Node(Actor *actor_, Tile *tile_, Node *previous_)
{
	actor= actor_;
	tile= tile_;
	previous= previous_;
}

Tile * Node::GetTile()
{
	return tile;
}

vector<Edge *> Node::GetEdges()
{
	vector<Edge *> edges;

	vector<Move *> moves= actor->GetMoves(tile);
	
	for(auto move : moves)
		edges.push_back(new Edge(new Node(actor, move->GetDestination(), this), move->GetCost()));

	return edges;
}

void Node::SetPrevious(Node *previous_)
{
	previous= previous_;
}

vector<Node *> Node::GetPath()
{
	vector<Node *> path;

	if(previous!= nullptr)
		path= previous->GetPath();

	path.push_back(this);

	return path;
}

float Node::GetPathCost()
{
	if(previous== nullptr)
		return 0;
	
	return 1+ previous->GetPathCost();
}

float Node::GetHeuristicCost(Node *target)
{
	return GetPathCost()+ tile->GetPosition().Distance(target->tile->GetPosition());
}

bool Node::operator==(const Node &other)
{
	return tile== other.tile;
}
