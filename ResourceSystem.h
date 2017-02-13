#ifndef EBR_RESOURCE_SYSTEM
#define EBR_RESOURCE_SYSTEM

#include "System.h"
#include "Common.h"


class Resource;

class ResourceSystem : public System
{
	vector<Resource *> resources;

protected:
	void Initialize();
	void Terminate();

public:
	void RegisterResource(Resource *resource);
	void DeregisterResource(Resource *resource);

	Resource * GetResource(string name);

	friend System;
};

#endif