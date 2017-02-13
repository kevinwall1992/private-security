#include "Pathing.h"

Path::Path(vector<Node*> nodes_)
{
	nodes= nodes_;
}

Node * Path::PopNode()
{
	Node *node= nodes[0];
	nodes.erase(nodes.begin());
	
	return node;
}

Path Path::GetPath(Node *starting_node, Node *ending_node)
{
	vector<Node *> open_set;
	vector<Node *> closed_set;
	open_set.push_back(starting_node);

	while(open_set.size()> 0)
	{
		Node *node= nullptr;

		for(unsigned int i= 0; i< open_set.size(); i++)
			if(node== nullptr || open_set[i]->GetHeuristicCost(ending_node)< node->GetHeuristicCost(ending_node))
				node= open_set[i];

		vector<Edge *> edges= node->GetEdges();
		for(unsigned int i= 0; i< edges.size(); i++)
		{
			Node *destination= edges[i]->GetDestination();

			if(destination->GetHeuristicCost(ending_node)> node->GetHeuristicCost(ending_node))
				continue;

			if(destination== ending_node)
				return destination->GetPath();

			destination->SetPrevious(node);
			open_set.push_back(destination);
		}
	}

	return vector<Node *>();
}
