#include "System.h"


bool System::systems_initialized= false;
SupportSystem System::support;
EmbreeSystem System::embree;
GraphicsSystem System::graphics;
ResourceSystem System::resource;

void System::InitializeSystems()
{
	support.Initialize();
	embree.Initialize();
	graphics.Initialize();
	resource.Initialize();
}

void System::TerminateSystems()
{
	graphics.Terminate();
	embree.Terminate();
	support.Terminate();
	resource.Terminate();
}