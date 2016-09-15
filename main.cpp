#include "Common.h"
#include "System.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Timer.h"


const int print_frame_count= 20;
int frame_set_count= 1;

int main(int argument_count, char **arguments)
{
	cout << std::fixed << std::setprecision(1);

	System::InitializeSystems();

	{
		Scene scene;
		scene.AddProps(Prop::ParseOBJ("test_scene_1.obj"));
		scene.AddLight(new AmbientLight(Color(0.2f, 0.2f, 0.2f)));
		scene.AddLight(new PointLight(Vec3f(26.5f, 45.0f, -30.0f), Color(0.8f, 0.8f, 0.8f)));
		scene.Commit();

		Camera camera(60, Vec3f(13.70f, 26.70, -66.64f));
		camera.LookAt(Vec3f(27.67f, 5.39f, 0.18f));
		Film film(System::graphics.screen_width, System::graphics.screen_height);
		camera.LoadFilm(&film);


		Timer display_timer;
		Timer render_timer;

		while(true)
		{
			display_timer.Start();
			render_timer.Start();

			Image *image= camera.TakePicture(scene);
			render_timer.Pause();
		
			System::graphics.Display(*image);
			display_timer.Pause();
		
			if((System::graphics.GetFrameCount()% print_frame_count)== 0)
			{
				float seconds_to_render= render_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float seconds_to_display= display_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
	
				float get_rays_ms= 1000* Timer::get_rays_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float embree_ms= 1000* Timer::embree_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float shading_ms= 1000* Timer::shading_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float develop_ms= 1000* Timer::develop_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float pre_shading_ms= 1000* Timer::pre_shading_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);
				float shadow_ms= 1000* Timer::shadow_timer.GetElapsedSeconds()/ ((float)print_frame_count* frame_set_count);

				float other_ms= (seconds_to_render* 1000)- (get_rays_ms+ embree_ms+ shading_ms+ develop_ms+ pre_shading_ms+ shadow_ms);

#if PRINT_AVERAGES
				frame_set_count++;
#endif

				cout << get_rays_ms << "ms/" << embree_ms << "ms/" << shading_ms << "ms/" << pre_shading_ms << "ms/" << shadow_ms << "ms," << "o:" << other_ms << "ms,";
				cout << "Render:" << 1/ seconds_to_render << "fps/" << seconds_to_render* 1000 << "ms\n";
				//cout << ",Display:" << 1/ seconds_to_display << "fps/" << seconds_to_display* 1000 << "ms\n";
			}

			bool exit_requested= false;
			SDL_Event event_;
			while(SDL_PollEvent(&event_))
				if(event_.type== SDL_QUIT)
					exit_requested= true;
			if(exit_requested)
				break;

			if((System::graphics.GetFrameCount()% (print_frame_count* 15))== 0)
				break;
		}
	}

	System::TerminateSystems();

	return 0;
}