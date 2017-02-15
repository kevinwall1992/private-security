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
		int node_index;

		for(unsigned int i= 0; i< open_set.size(); i++)
		{
			if(node== nullptr || open_set[i]->GetHeuristicCost(ending_node)< node->GetHeuristicCost(ending_node))
			{
				node= open_set[i];
				node_index= i;
			}
		}
		open_set.erase(open_set.begin()+ node_index);
		closed_set.push_back(node);


		vector<Edge *> edges= node->GetEdges();
		for(unsigned int i= 0; i< edges.size(); i++)
		{
			Node *destination= edges[i]->GetDestination();

			if(*destination== *ending_node)
				return destination->GetPath();

			bool already_exists= false;
			for(unsigned int j= 0; j< open_set.size() && already_exists== false; j++)
				if(*open_set[j]== *destination)
					already_exists= true;
			for(unsigned int j= 0; j< closed_set.size() && already_exists== false; j++)
				if(*closed_set[j]== *destination)
					already_exists= true;

			if(!already_exists)
				open_set.push_back(destination);
		}
	}

	return vector<Node *>();
}
