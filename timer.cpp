#include "Timer.h"

#include <SDL.h>


Timer Timer::get_rays_timer;
Timer Timer::embree_timer;
Timer Timer::primary_shading_timer;
Timer Timer::secondary_shading_timer;
Timer Timer::develop_timer;
Timer Timer::pre_shading_timer;
Timer Timer::shadow_timer;

void Timer::AddTime()
{
	unsigned int ticks= SDL_GetTicks();
	elapsed_milliseconds+= ticks- last_ticks;
	last_ticks= ticks;
}

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

	AddTime();

	is_paused= true;

	return GetElapsedSeconds();
}

float Timer::Stop()
{
	if(!is_paused && !is_stopped)
		AddTime();

	is_stopped= true;
	is_paused= false;

	return GetElapsedSeconds();
}

unsigned int Timer::GetElapsedMilliseconds()
{
	if(!is_paused && !is_stopped)
		AddTime();

	return elapsed_milliseconds;
}

float Timer::GetElapsedSeconds()
{
	return GetElapsedMilliseconds()/ 1000.0f;
}