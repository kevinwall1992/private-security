#include "Resource.h"
#include "ResourceSystem.h"


Resource::Resource(string name)
{
	this->name= name;

	System::resource.RegisterResource(this);
}

Resource::~Resource()
{
	System::resource.DeregisterResource(this);
}

string Resource::GetName()
{
	return name;
}