#include "System.h"
#include "EmbreeSystem.h"
#include "GraphicsSystem.h"


bool System::systems_initialized= false;
EmbreeSystem System::embree;
GraphicsSystem System::graphics;

void System::InitializeSystems()
{
	embree.Initialize();
	graphics.Initialize();
}

void System::TerminateSystems()
{
	graphics.Terminate();
	embree.Terminate();
}