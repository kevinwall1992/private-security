#include "Node.h"

Node::Node(Actor * actor, Tile * tile)
{
	this->actor= actor;
	this->tile= tile;
}

Tile * Node::GetTile()
{
	return tile;
}

vector<Edge *> Node::GetEdges()
{
	vector<Edge *> edges;

	vector<Move *> moves= actor->GetMoves();
	vector<Move *> enabled_moves= actor->GetTile()->GetMoves();
	for(unsigned int i= 0; i< enabled_moves.size(); i++)
		moves.push_back(enabled_moves[i]);
	
	for(unsigned int i= 0; i< moves.size(); i++)
		if(!(actor->DoesBlock(moves[i]) || actor->GetTile()->DoesBlock(moves[i])))
			edges.push_back(new Edge(new Node(actor, moves[i]->GetDestination()), 1));

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

bool Node::operator==(Node *other)
{
	if(other== nullptr)
		return false;

	return tile== other->tile;
}
