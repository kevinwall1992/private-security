#include <iostream>

#include "Space.h"
#include "RasterCamera.h"
#include "RayCamera.h"
#include "OpenGLUtility.h"
#include "InputSystem.h"
#include "DictionaryFile.h"

#include "EBRMain.h"


int main(int argument_count, char **arguments)
{
	//return EBR_main(argument_count, arguments);

	System::InitializeSystems();

	Scene scene;
	vector<Mesh *> meshes= Mesh::Parse("cornell.obj");
	for(unsigned int i= 0; i< meshes.size(); i++)
		scene.AddProp(new MeshProp(meshes[i]));
	//scene.GetProps()[2]->RemoveDrawFlags(Prop::DrawFlags::RasterizeGbuffers);
	//scene.GetProps()[5]->TurnOff();
	//Prop *prop= new MeshProp(Mesh::Retrieve("guanyiny.obj", "guanyinyy"));
	//prop->RemoveDrawFlags(Prop::DrawFlags::RasterizeGbuffers);
	//scene.AddProp(prop);

	RasterCamera camera(DegreesToRadians(38.5), Vec3f(23.8f, 22.8f, -116.5f));
	camera.LookAt(Vec3f(23.8f, 22.8f, 0.0f));

	scene.AddLight(new PointLight(Vec3f(23.8f, 45.0f, -35.0f), Color(0.92f, 0.80f, 0.65f)));
	scene.Commit();

	CameraGizmo *camera_gizmo= new CameraGizmo(&camera);
	System::input.AddGizmo(camera_gizmo);

	while(System::input.HandleInput())
		System::graphics.Display(camera.TakePhoto(scene, System::graphics.GetScreenWidth(), System::graphics.GetScreenHeight()));

	System::TerminateSystems();
	return 0;
}