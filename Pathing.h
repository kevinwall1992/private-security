#ifndef PS_PATHING
#define PS_PATHING


#include "Node.h"

class Path
{
	vector<Node *> nodes;

	Path(vector<Node *> nodes);

public:

	Node * PopNode();

	static Path GetPath(Node *starting_node, Node *ending_node);
};

#endif