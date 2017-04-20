#ifdef USE_EBR

#include "Common.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "Gizmos.h"
#include "Scene.h"
#include "RayCamera.h"
#include "Light.h"
#include "Timer.h"
#include "Pane.h"

#include "Catalog.h"

#include <Windows.h>
#include <iomanip>


const int print_frame_count = 10;
int frame_set_count = 1;

#define CORNELL 1

int EBR_main(int argument_count, char **arguments)
{
	DisableProcessWindowsGhosting();
	srand(0);

	cout << std::fixed << std::setprecision(1);

	System::InitializeSystems();

	{
		Scene scene;

#if CORNELL
		vector<Mesh *> meshes= Mesh::Parse("cornell.obj");
		scene.AddProp(new MeshProp(Mesh::Retrieve("guanyiny.obj", "guanyinyy")));
		scene.AddLight(new PointLight(Vec3f(23.8f, 45.0f, -30.0f), Color(0.92f, 0.80f, 0.65f)));
#else
		vector<Mesh *> meshes= Mesh::Parse("test_scene_1.obj");
		scene.AddLight(new PointLight(Vec3f(26.5f, 45.0f, -30.0f), Color(0.9f, 0.9f, 0.9f)));
		//scene.AddLight(new PointLight(Vec3f(36.5f, 35.0f, -30.0f), Color(0.9f, 0.9f, 0.9f)));
#endif
		for(unsigned int i= 0; i< meshes.size(); i++)
			scene.AddProp(new MeshProp(meshes[i]));

		//scene.AddLight(new AmbientLight(Color(0.1f, 0.1f, 0.1f)));
		//scene.AddLight(new PointLight(Vec3f(33.5f, 1.0f, -25.0f), Color(0.4f, 0.4f, 0.4f)));
		//scene.AddLight(new PointLight(Vec3f(13.70f, 26.70, -66.64f), Color(0.5f, 0.5f, 0.5f)));
		scene.Commit();

#if CORNELL
		RayCamera camera(Math::DegreesToRadians(38.5), Vec3f(23.8f, 22.8f, -116.5f));
		camera.LookAt(Vec3f(23.8f, 22.8f, 0.0f));
#else
		RayCamera camera(DegreesToRadians(60), Vec3f(13.70f, 26.70f, -66.64f));
		camera.LookAt(Vec3f(27.67f, 5.39f, 0.18f));
#endif
		System::input.AddGizmo(new CameraGizmo(&camera));

		CameraPane camera_pane(&camera);
		camera_pane.SetScene(&scene);

		Timer display_timer;
		Timer render_timer;

		while(System::input.HandleInput())
		{
			display_timer.Start();
			render_timer.Start();

			//Photo photo= camera.TakePhoto(scene, System::graphics.GetScreenSize(), Photo::Type::FullColor);
			System::graphics.Display(&camera_pane);
			render_timer.Pause();
			display_timer.Pause();

			if ((System::graphics.GetFrameCount() % print_frame_count) == 0)
			{
#if PRINT_AVERAGES == 0
				float seconds_to_render = render_timer.Stop()/ ((float)print_frame_count);
				float seconds_to_display = display_timer.Stop()/ ((float)print_frame_count);

				float get_rays_ms = 1000 * Timer::get_rays_timer.Stop()/ ((float)print_frame_count);
				float embree_ms = 1000 * Timer::embree_timer.Stop()/ ((float)print_frame_count);
				float primary_shading_ms = 1000 * Timer::primary_shading_timer.Stop()/ ((float)print_frame_count);
				float secondary_shading_ms = 1000 * Timer::secondary_shading_timer.Stop()/ ((float)print_frame_count);
				float develop_ms = 1000 * Timer::develop_timer.Stop()/ ((float)print_frame_count);
				float pre_shading_ms = 1000 * Timer::pre_shading_timer.Stop()/ ((float)print_frame_count);
				float shadow_ms = 1000 * Timer::shadow_timer.Stop()/ ((float)print_frame_count);

				float other_ms = (seconds_to_render * 1000) - (get_rays_ms + embree_ms + primary_shading_ms + secondary_shading_ms + develop_ms + pre_shading_ms + shadow_ms);

#else
				float seconds_to_render = render_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float seconds_to_display = display_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);

				float get_rays_ms = 1000 * Timer::get_rays_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float embree_ms = 1000 * Timer::embree_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float primary_shading_ms = 1000 * Timer::primary_shading_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float secondary_shading_ms = 1000 * Timer::secondary_shading_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float develop_ms = 1000 * Timer::develop_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float pre_shading_ms = 1000 * Timer::pre_shading_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float shadow_ms = 1000 * Timer::shadow_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);

				float other_ms = (seconds_to_render * 1000) - (get_rays_ms + embree_ms + primary_shading_ms + secondary_shading_ms + develop_ms + pre_shading_ms + shadow_ms);
#endif

#if PRINT_AVERAGES
				frame_set_count++;
#endif

				cout << get_rays_ms << "ms/" << embree_ms << "ms/" << primary_shading_ms << "ms/" << secondary_shading_ms << "ms/" << pre_shading_ms << "ms/" << shadow_ms << "ms," << "o:" << other_ms << "ms,";
				cout << "Render:" << 1 / seconds_to_render << "fps/" << seconds_to_render * 1000 << "ms\n";
			}

			if (false)
				if (System::graphics.GetFrameCount() >= 100)
				{
					getchar();
					break;
				}

			//if((System::graphics.GetFrameCount()% (print_frame_count* 15))== 0)
			//	break;
		}
	}

	System::TerminateSystems();

	return 0;
}

#endif