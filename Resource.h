#ifndef EBR_RESOURCE
#define EBR_RESOURCE

#include "Common.h"

class Resource : public Polymorphic
{
	string name;

public:
	Resource(string name);
	~Resource();

	string GetName();
};

#endif