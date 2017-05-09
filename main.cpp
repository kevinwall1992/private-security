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

#include "EBRMain.h"


int main(int argument_count, char **arguments)
{
	//return EBR_main(argument_count, arguments);

	System::InitializeSystems();

#if 0
	Scene scene;
	vector<Mesh *> meshes= Mesh::Parse("cornell.obj");
	for(unsigned int i= 0; i< meshes.size(); i++)
		scene.AddProp(new MeshProp(meshes[i]));
	//scene.GetProps()[2]->RemoveDrawFlags(Prop::DrawFlags::RasterizeGbuffers);
	//scene.GetProps()[5]->TurnOff();
	Prop *prop= new MeshProp(Mesh::Retrieve("guanyiny.obj", "guanyinyy"));
	prop->RemoveDrawFlags(Prop::DrawFlags::RasterizeGbuffers);
	scene.AddProp(prop);

	RasterCamera camera(DegreesToRadians(38.5), Vec3f(23.8f, 22.8f, -116.5f));
	camera.LookAt(Vec3f(23.8f, 22.8f, 0.0f));

	scene.AddLight(new PointLight(Vec3f(23.8f, 45.0f, -35.0f), Color(0.92f, 0.80f, 0.65f)));
	scene.Commit();
#else
	Actor *actor= new Actor();

	/*System::game.space.GetTile(0, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(0, 3, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(0, 4, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(0, 5, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(0, 6, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(0, 7, 0)->PutFurniture(new ThickWall());

	System::game.space.GetTile(0, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(1, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(2, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(3, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(4, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(5, 2, 0)->PutFurniture(new ThickWall());*/

	System::game.space.GetTile(0, 0, 0)->PutFurniture(new Museum());

	System::game.space.GetTile(5, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(6, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(7, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(8, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(10, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(11, 18, 0)->PutFurniture(new InvisibleThickWall());

	System::game.space.GetTile(12, 18, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(12, 19, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(12, 20, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(12, 21, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(12, 23, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(12, 24, 0)->PutFurniture(new InvisibleThickWall());

	System::game.space.GetTile(12, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(11, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(10, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(9, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(7, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(6, 25, 0)->PutFurniture(new InvisibleThickWall());

	System::game.space.GetTile(5, 25, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(5, 24, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(5, 23, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(5, 22, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(5, 20, 0)->PutFurniture(new InvisibleThickWall());
	System::game.space.GetTile(5, 19, 0)->PutFurniture(new InvisibleThickWall());

	System::game.space.GetTile(8, 21, 0)->PutActor(actor);

	Scene scene;
	scene.AddProp(&System::game.space);
	
	RasterCamera camera(100, Vec3f(2, 1.5, 2), Math::DegreesToRadians(-15), Math::DegreesToRadians(80));
	//RasterCamera camera(Math::DegreesToRadians(60), Vec3f(1, 3, 5));
	//camera.LookAt(Vec3f(2, 1, 2));
	
	scene.AddLight(new PointLight(Vec3f(16.0f, 25.0f, 10.0f), Color(0.92f, 0.85f, 0.77f)));
	scene.Commit();
#endif

	TacticalInterface *tactical_interface= new TacticalInterface(&camera);
	tactical_interface->SetScene(&scene);
	System::input.AddGizmo(tactical_interface);


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

		System::graphics.Display(tactical_interface);
	}

	System::TerminateSystems();
	return 0;
}