#include "Resource.h"
#include "System.h"


Resource::Resource(string name_)
{
	name= name_;

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