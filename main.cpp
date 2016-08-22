#include "Common.h"
#include "System.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Timer.h"


const int print_frame_count= 40;

int main(int argument_count, char **arguments)
{
	cout << std::fixed << std::setprecision(1);

	System::InitializeSystems();

	{
		Scene scene;
		scene.AddOBJ("teapot3.obj");
		scene.AddLight(new AmbientLight(Color(0.5f, 0.5f, 0.5f)));
		scene.AddLight(new PointLight(Vec3f(0.0f, 0.0f, 0.0f), Color(0.5f, 0.5f, 0.5f)));
		scene.Commit();

		Camera camera(90, Vec3f(0.0f, 0.0f, -3.0f));
		camera.LookAt(Vec3f(0.0f, 0.0f, 10.0f));
		Film film(System::graphics.screen_width, System::graphics.screen_height);
		camera.LoadFilm(&film);


		Timer display_timer;
		Timer render_timer;

		while(true)
		{
			display_timer.Start();
			render_timer.Start();

			Image image= camera.TakePicture(scene);
			render_timer.Pause();
		
			System::graphics.Display(image);
			display_timer.Pause();
		
			if((System::graphics.GetFrameCount()% print_frame_count)== 0)
			{
				float seconds_to_render= render_timer.Stop()/ (float)print_frame_count;
				float seconds_to_display= display_timer.Stop()/ (float)print_frame_count;

				cout << "Render FR/FT: " << 1/ seconds_to_render << "fps/" << seconds_to_render* 1000 << "ms";
				cout << ", Display FR/FT: " << 1/ seconds_to_display << "fps/" << seconds_to_display* 1000 << "ms\n";
			}

			bool exit_requested= false;
			SDL_Event event_;
			while(SDL_PollEvent(&event_))
				if(event_.type== SDL_QUIT)
					exit_requested= true;
			if(exit_requested)
				break;

			if((System::graphics.GetFrameCount()% (print_frame_count* 30))== 0)
				break;
		}
	}

	System::TerminateSystems();

	return 0;
}