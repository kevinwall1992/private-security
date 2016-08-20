#include "System.h"
#include "EmbreeSystem.h"
#include "GraphicsSystem.h"


bool System::systems_initialized= false;
SupportSystem System::support;
EmbreeSystem System::embree;
GraphicsSystem System::graphics;

void System::InitializeSystems()
{
	support.Initialize();
	embree.Initialize();
	graphics.Initialize();
}

void System::TerminateSystems()
{
	graphics.Terminate();
	embree.Terminate();
	support.Terminate();
}