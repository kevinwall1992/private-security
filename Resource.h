#ifndef EBR_RESOURCE
#define EBR_RESOURCE

#include "Common.h"

class Resource : public Polymorphic
{
	string name;

protected:
	Resource(string name= "");
	~Resource();

public:
	string GetName();
};

#endif