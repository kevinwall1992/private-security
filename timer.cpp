#include "Timer.h"
#include "GraphicsHeaders.h"


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
	unsigned int ticks= SDL_GetTicks();
	int difference= ticks- last_ticks;
	elapsed_milliseconds+= difference;

	is_paused= true;

	return difference/ 1000.0f;
}

float Timer::Stop()
{
	if(!is_paused)
		elapsed_milliseconds+= SDL_GetTicks()- last_ticks;

	is_stopped= true;

	return GetElapsedSeconds();
}

float Timer::GetElapsedSeconds()
{
	return elapsed_milliseconds / 1000.0f;
}