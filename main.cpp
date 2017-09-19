#include <iostream>

#include "Space.h"
#include "RasterCamera.h"
#include "RayCamera.h"
#include "InputSystem.h"
#include "Gizmos.h"
#include "DictionaryFile.h"
#include "Pathing.h"
#include "GameSystem.h"
#include "Actions.h"
#include "UI.h"
#include "Drawables.h"
#include "GarbageCan.h"

#include "EBRMain.h"


int main(int argument_count, char **arguments)
{
	//return EBR_main(argument_count, arguments);

	System::InitializeSystems();

	DevelopmentInterface development_interface;
	System::input.AddGizmo(&development_interface);


	Timer timer;
	int last_ms= 0;
	timer.Start();

	while(System::input.HandleInput())
	{
		int ms= timer.GetElapsedMilliseconds();
		int difference= ms- last_ms;
		last_ms= ms;

		if(difference> 0)
			System::game.Step(difference);

		System::graphics.Display(&development_interface);

		Landfill::Dump();
	}

	System::TerminateSystems();
	return 0;
}