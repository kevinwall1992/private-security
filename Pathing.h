#ifndef PS_PATHING
#define PS_PATHING


#include "Node.h"

class Path
{
	vector<Edge *> edges;

	Path(vector<Edge *> edges);

public:

	Edge * PopEdge();

	static Path GetPath(Node *starting_node, Node *ending_node);
};

#endif