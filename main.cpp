#include "Common.h"
#include "System.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"

#include <Windows.h>


int main(int argument_count, char **arguments)
{
	System::InitializeSystems();

	cout << std::fixed << std::setprecision(1);

	Scene scene;
	scene.AddOBJ("teapot3.obj");
	scene.AddLight(new AmbientLight(Color(0.5f, 0.5f, 0.5f)));
	scene.AddLight(new PointLight(Vec3f(0.0f, 0.0f, 0.0f), Color(0.5f, 0.5f, 0.5f)));
	scene.Commit();

	Camera camera(90, Vec3f(0.0f, 0.0f, -3.0f));
	camera.LookAt(Vec3f(0.0f, 0.0f, 10.0f));
	Film film(System::graphics.screen_width, System::graphics.screen_height);
	camera.LoadFilm(&film);

	int start_ticks= SDL_GetTicks();
	int last_ticks= start_ticks;
	int total_frame_count= 0;
	int print_frame_count= 40;
	int total_render_milliseconds= 0;
	while(true)
	{
		int before_render_ticks= SDL_GetTicks();
		camera.TakePicture(scene);
#if PARALLEL_DEVELOP
#else
		camera->film->Develop(image);
#endif
		total_render_milliseconds+= SDL_GetTicks()- before_render_ticks;
		if((++total_frame_count% print_frame_count)== 0)
		{
			float average_frametime= total_render_milliseconds/ (float)print_frame_count;
			cout << "Render FT/FR: " << 1000/ average_frametime << "fps/" << average_frametime << "ms";
			total_render_milliseconds= 0;
		}

		System::graphics.Render(film.image);
		
		if((total_frame_count% print_frame_count)== 0)
		{
			int current_ticks= SDL_GetTicks();
			float seconds= (current_ticks- last_ticks)/ 1000.0f;
			cout << ", Display FR/FT/AVFT: " << print_frame_count/ seconds << "fps/" << 1000* seconds/ print_frame_count << "ms/" << (current_ticks- start_ticks)/ (float)total_frame_count << "ms\n";
			last_ticks= current_ticks;
		}
		

		bool exit_requested= false;
		SDL_Event event_;
		while(SDL_PollEvent(&event_))
			if(event_.type== SDL_QUIT)
				exit_requested= true;
		if(exit_requested)
			break;

		if((total_frame_count% (print_frame_count* 15))== 0)
			break;
	}

	System::TerminateSystems();

	return 0;
}