#include <iostream>

#include "Space.h"
#include "RasterCamera.h"
#include "RayCamera.h"
#include "OpenGLUtility.h"
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

	System::game.space.GetTile(1, 1, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(1, 2, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(2, 1, 0)->PutFurniture(new ThickWall());
	System::game.space.GetTile(3, 1, 1)->PutFurniture(new ThickWall());
	System::game.space.GetTile(2, 2, 0)->PutActor(actor);
	
	Path path= Path::GetPath(new Node(actor, actor->GetTile()), new Node(actor, System::game.space.GetTile(0, 0, 0)));

	Edge *edge;
	while((edge= path.PopEdge())!= nullptr)
		System::game.SubmitAction(new MoveAction(actor, edge->GetMove()));

	Scene scene;
	scene.AddProp(&System::game.space);
	
	RasterCamera camera(Math::DegreesToRadians(60), Vec3f(0, 0, 5));
	camera.LookAt(Vec3f(-2, 2, 0));
	
	scene.AddLight(new PointLight(Vec3f(3.3f, 7.0f, 8.0f), Color(0.92f, 0.80f, 0.65f)));
	//scene.Commit();
#endif

	TacticalPane *tactical_pane= new TacticalPane(&camera);
	tactical_pane->SetScene(&scene);
	tactical_pane->Size= Vec2f(0.8f, 0.8f);
	tactical_pane->Offset= Vec2f(0.1f, 0.1f);
	System::input.AddGizmo(tactical_pane);


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

		System::graphics.Display(tactical_pane);
	}

	System::TerminateSystems();
	return 0;
}