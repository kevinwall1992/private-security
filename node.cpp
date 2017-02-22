#include "Node.h"

Node::Node(Actor *actor_, Tile *tile_)
{
	actor= actor_;
	tile= tile_;
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
		edges.push_back(new Edge(this, new Node(actor, move->GetDestination()), move));

	return edges;
}

void Node::SetInEdge(Edge *in_)
{
	in= in_;
}

vector<Edge *> Node::GetPath()
{
	vector<Edge *> path;

	if(in!= nullptr)
	{
		path= in->GetSource()->GetPath();
		path.push_back(in);
	}

	return path;
}

float Node::GetPathCost()
{
	if(in== nullptr)
		return 0;
	
	return in->GetCost()+ in->GetSource()->GetPathCost();
}

float Node::GetHeuristicCost(Node *target)
{
	return GetPathCost()+ tile->GetPosition().Distance(target->tile->GetPosition());
}

bool Node::operator==(const Node &other)
{
	return tile== other.tile;
}
