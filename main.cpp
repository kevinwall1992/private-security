#include <iostream>

#include "Space.h"
#include "RasterCamera.h"
#include "RayCamera.h"
#include "OpenGLUtility.h"
#include "InputSystem.h"
#include "DictionaryFile.h"
#include "Pathing.h"
#include "GameSystem.h"
#include "Actions.h"

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

	System::game.space.tiles[1][1][0].PutFurniture(new ThickWall());
	System::game.space.tiles[1][2][0].PutFurniture(new ThickWall());
	System::game.space.tiles[2][1][0].PutFurniture(new ThickWall());
	System::game.space.tiles[3][1][1].PutFurniture(new ThickWall());
	System::game.space.tiles[2][2][0].PutActor(actor);
	
	Path path= Path::GetPath(new Node(actor, actor->GetTile()), new Node(actor, &System::game.space.tiles[0][0][0]));

	Edge *edge;
	while((edge= path.PopEdge())!= nullptr)
		System::game.SubmitAction(new MoveAction(actor, edge->GetMove()));

	Scene scene;
	scene.AddProp(&System::game.space);

	RasterCamera camera(DegreesToRadians(60), Vec3f(0, 0, 5));
	camera.LookAt(Vec3f(-2, 2, 0));

	scene.AddLight(new PointLight(Vec3f(3.3f, 7.0f, 8.0f), Color(0.92f, 0.80f, 0.65f)));
	scene.Commit();
#endif

	CameraGizmo *camera_gizmo= new CameraGizmo(&camera);
	System::input.AddGizmo(camera_gizmo);

	while(System::input.HandleInput())
	{
		System::game.Step(10);
		System::graphics.Display(camera.TakePhoto(scene, System::graphics.GetScreenWidth(), System::graphics.GetScreenHeight()));
	}

	System::TerminateSystems();
	return 0;
}