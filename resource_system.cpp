#include "ResourceSystem.h"
#include "Resource.h"


void ResourceSystem::Initialize()
{

}

void ResourceSystem::Terminate()
{
	
}

void ResourceSystem::RegisterResource(Resource *resource)
{
	resources.push_back(resource);
}

//Slow obviously, wll want something better if we are constantly registering and deregistering
void ResourceSystem::DeregisterResource(Resource *resource)
{
	for(unsigned int i= 0; i< resources.size(); i++)
		if(resources[i]== resource)
			resources.erase(resources.begin()+ i);
}

Resource * ResourceSystem::GetResource(string name)
{
	for(unsigned int i= 0; i< resources.size(); i++)
		if(resources[i]->GetName()== name)
			return resources[i];

	return nullptr;
}