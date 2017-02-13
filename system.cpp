#include "System.h"
#include "SupportSystem.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "EmbreeSystem.h"
#include "ResourceSystem.h"


bool System::systems_initialized= false;
SupportSystem System::support;
EmbreeSystem System::embree;
GraphicsSystem System::graphics;
InputSystem System::input;
ResourceSystem System::resource;

void System::InitializeSystems()
{
	support.Initialize();
	embree.Initialize();
	graphics.Initialize();
	input.Initialize();
	resource.Initialize();
}

void System::TerminateSystems()
{
	resource.Terminate();
	input.Terminate();
	graphics.Terminate();
	embree.Initialize();
	support.Terminate();
}