#include "Timer.h"

#include <SDL.h>


Timer Timer::get_rays_timer;
Timer Timer::embree_timer;
Timer Timer::primary_shading_timer;
Timer Timer::secondary_shading_timer;
Timer Timer::develop_timer;
Timer Timer::pre_shading_timer;
Timer Timer::shadow_timer;

void Timer::Start()
{
	if(is_stopped || is_paused)
	{
		if(is_stopped)
		{
			elapsed_milliseconds= 0;
			is_stopped= false;
		}
		else
			is_paused= false;

		last_ticks= SDL_GetTicks();
	}
}

float Timer::Pause()
{
	if(is_paused || is_stopped)
		return 0.0f;

	unsigned int ticks= SDL_GetTicks();
	int difference= ticks- last_ticks;
	elapsed_milliseconds+= difference;

	is_paused= true;

	return difference/ 1000.0f;
}

float Timer::Stop()
{
	if(!is_paused && !is_stopped)
		elapsed_milliseconds+= SDL_GetTicks()- last_ticks;

	is_stopped= true;
	is_paused= false;

	return GetElapsedSeconds();
}

//this should probably add time if you aren't paused or stopped
float Timer::GetElapsedSeconds()
{
	return elapsed_milliseconds / 1000.0f;
}