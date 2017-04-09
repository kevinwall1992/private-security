#include "Cycle.h"
#include "Common.h"

#include <SDL.h>


float Cycle::GetLength()
{
	return length;
}

float Cycle::GetLastMoment()
{
	return last_moment;
}

float Cycle::GetLastVelocity()
{
	return last_velocity;
}

void Cycle::SetLastMoment(float moment, float velocity)
{
	last_moment= moment;
	last_velocity= velocity;
}

float Cycle::GetElapsedTime(unsigned int ticks)
{
	return ticks/ 1000.0f;
}

float Cycle::GetElapsedCycles(unsigned int ticks)
{
	return GetElapsedTime(ticks)/ length;
}

float Cycle::GetMoment(unsigned int ticks)
{
	float elapsed_cycles= GetElapsedCycles(ticks);

	return elapsed_cycles- (int)elapsed_cycles;
}

float Cycle::ElapsedTimeToElapsedCycles(float elapsed_time)
{
	return elapsed_time/ length;
}

float Cycle::ElapsedTimeToMoment(float elapsed_time)
{
	return ElapsedCyclesToMoment(ElapsedTimeToElapsedCycles(elapsed_time));
}

float Cycle::ElapsedCyclesToMoment(float elapsed_cycles)
{
	return elapsed_cycles- (int)elapsed_cycles;
}

void Cycle::Reset()
{
	last_moment= 0;
	last_velocity= 0;
}

Cycle::Cycle(float length_)
{
	length= length_;
}

float Cycle::GetMoment()
{
	return GetMoment(SDL_GetTicks_());
}

unsigned int Cycle::SDL_GetTicks_()
{
	return SDL_GetTicks()/ 1;
}

StartStopCycle::StartStopCycle(float length, int start_ticks_, bool is_capped_, float pause_moment_, float pause_elapsed_time_)
	: Cycle(length)
{
	start_ticks= start_ticks_;
	is_capped= is_capped_;
	pause_moment= pause_moment_;
	pause_elapsed_time= pause_elapsed_time_;
}

float StartStopCycle::GetElapsedTime(unsigned int ticks)
{
	return pause_elapsed_time+ Cycle::GetElapsedTime(ticks);
}

float StartStopCycle::GetElapsedCycles(unsigned int ticks)
{
	return pause_elapsed_time+ Cycle::GetElapsedCycles(ticks);
}

int StartStopCycle::GetStartTicks()
{
	return start_ticks;
}

bool StartStopCycle::IsCapped()
{
	return is_capped;
}

bool StartStopCycle::IsStopping()
{
	return is_stopping;
}

bool StartStopCycle::IsPaused()
{
	return is_paused;
}

float StartStopCycle::GetPauseMoment()
{
	return pause_moment;
}

float StartStopCycle::GetPauseElapsedTime()
{
	return pause_elapsed_time;
}

float StartStopCycle::GetPauseElapsedCycles()
{
	return ElapsedTimeToElapsedCycles(pause_elapsed_time);
}

float StartStopCycle::GetStopMoment()
{
	return stop_moment;
}

float StartStopCycle::GetStopElapsedTime()
{
	return stop_elapsed_time;
}

float StartStopCycle::GetStopElapsedCycles()
{
	return ElapsedTimeToElapsedCycles(stop_elapsed_time);
}

void StartStopCycle::Reset()
{
	start_ticks= SDL_GetTicks_();

	pause_moment= 0.0f;
	pause_elapsed_time= 0.0f;
	stop_moment= 0.0f;
	stop_elapsed_time= 0.0f;

	Cycle::Reset();
}

StartStopCycle::StartStopCycle(float length, bool cap)
	: Cycle(length)
{
	is_capped= cap;

	is_paused= true;
}

void StartStopCycle::Start()
{
	unsigned int ticks= SDL_GetTicks_();

	pause_moment= GetMoment();

	start_ticks= ticks;

	is_paused= false;
	is_stopping= false;
}

void StartStopCycle::Stop()
{
	if(is_stopping)
		return;

	stop_moment= GetMoment();
	unsigned int ticks= SDL_GetTicks_();
	stop_elapsed_time= GetElapsedTime(ticks- GetStartTicks());
	start_ticks= ticks;

	is_stopping= true;
}

void StartStopCycle::Pause()
{
	pause_moment= GetMoment();
	unsigned int ticks= SDL_GetTicks_();
	pause_elapsed_time= GetElapsedTime(ticks- GetStartTicks());
	start_ticks= ticks;

	is_paused= true;
}

float StartStopCycle::GetMoment()
{
	assert(false && "StartStopCycle::GetMoment(): Not implemented");
	return 0;
}

SmoothCycle::SmoothCycle(float length, int start_ticks, bool is_capped, float pause_moment, float pause_elapsed_time, float pause_velocity_)
	: StartStopCycle(length, start_ticks, is_capped, pause_moment, pause_elapsed_time)
{
	pause_velocity= pause_velocity_;

	is_sub_cycle= true;
}

void SmoothCycle::Reset()
{
	stop_velocity= 0;
	pause_velocity= 0;

	StartStopCycle::Reset();
}

SmoothCycle::SmoothCycle(float length, float acceleration_length_)
	: StartStopCycle(length, false)
{
	acceleration_length= acceleration_length_;
}

SmoothCycle::SmoothCycle(float length, bool cap)
	: StartStopCycle(length, cap)
{
	acceleration_length= GetLength();
}

void SmoothCycle::Start()
{
	pause_velocity= GetLastVelocity();

	StartStopCycle::Start();

	if(sub_cycle!= nullptr)
	{
		delete sub_cycle;
		sub_cycle= nullptr;
	}
}

void SmoothCycle::Stop()
{
	stop_velocity= GetLastVelocity();

	StartStopCycle::Stop();
}

void SmoothCycle::Pause()
{
	pause_velocity= GetLastVelocity();

	StartStopCycle::Pause();
}

float SmoothCycle::GetMoment()
{
	if(IsPaused())
		return GetPauseMoment();

	if(IsCapped())
	{
		if(IsStopping())
		{
			if(GetStopMoment()== 0)
				return 0;

			unsigned int ticks= SDL_GetTicks_();
			float relative_length= (0.5f+ GetStopMoment())/ 2;
			
			if(sub_cycle== nullptr)
			{
				float stop_elapsed_cycles_capped= std::min(GetStopElapsedCycles(), 1.0f);
				sub_cycle= new SmoothCycle(relative_length* GetLength(), ticks, true, 0, 0, relative_length* -stop_velocity/ GetStopMoment());
			}
			
			float moment= (1- sub_cycle->GetMoment())* GetStopMoment();
			float velocity= GetStopMoment()* -sub_cycle->GetLastVelocity()/ relative_length;
			if(moment< 0)
			{
				moment= 0;
				velocity= 0;
			}
			SetLastMoment(moment, velocity);
			return moment;
		}
		else
		{
			unsigned int ticks= SDL_GetTicks_();
			unsigned int start_ticks= GetStartTicks();
			float elapsed_cycles= GetElapsedCycles(ticks- GetStartTicks());
			if(elapsed_cycles> 1)
			{
				SetLastMoment(1, 0);
				return 1;
			}

			float desired_interval= 1;

			float acceleration= (sqrt(4* pow(GetPauseMoment(), 2)
									 + 4* GetPauseMoment()* desired_interval* pause_velocity
									 - 8* GetPauseMoment()
									 + 2* pow(desired_interval* pause_velocity, 2)
									 - 4* desired_interval* pause_velocity+ 4)
								- 2* GetPauseMoment()
								- desired_interval* pause_velocity
								+ 2)/ (desired_interval* desired_interval);

			float t0= (acceleration* desired_interval- pause_velocity)/ (2* acceleration);
			float t1= desired_interval- t0;
			float h0= t0* acceleration;
			float h1= t1* acceleration;

			float current_t0= std::min(elapsed_cycles, t0);
			float current_t1= std::min(elapsed_cycles- current_t0, t1);
			float inverse_current_t1= t1* (1- current_t1/ t1);

			float moment= GetPauseMoment()
				          + acceleration* current_t0* current_t0/ 2
				          + pause_velocity* current_t0
				          + acceleration* (t1* t1- inverse_current_t1* inverse_current_t1)/ 2;

			float velocity= (pause_velocity+ acceleration* current_t0- acceleration* current_t1);

			if(!is_sub_cycle && moment< 0)
			{
				moment= 0;
				velocity= 0;
				Reset();
			}
			SetLastMoment(moment, velocity);
			return moment;
		}
	}
	else
	{
		if(IsStopping())
		{
			unsigned int ticks= SDL_GetTicks_();
			float linear_stop_moment= Cycle::ElapsedCyclesToMoment(GetStopElapsedCycles());
			bool reverse= linear_stop_moment< 0.35f;

			if(sub_cycle== nullptr)
			{
				if(reverse)
					sub_cycle= new SmoothCycle(linear_stop_moment* GetLength(), ticks, true, 0, 0, -stop_velocity/ linear_stop_moment/ 2);
				else
					sub_cycle= new SmoothCycle((1- linear_stop_moment)* GetLength(), ticks, true, 0, 0, stop_velocity/ (1- linear_stop_moment)/ 2);
			}
			
			float moment;
			if(reverse)
				moment= GetStopMoment()* (1- sub_cycle->GetMoment());
			else
				moment= GetStopMoment()+ sub_cycle->GetMoment()* (1- GetStopMoment());
			SetLastMoment(moment, (reverse ? -1 : 1)* sub_cycle->GetLastVelocity());
			return moment;
		}
		else
		{
			float elapsed_time= GetElapsedTime(SDL_GetTicks_()- GetStartTicks());

			float acceleration_interval= std::min(elapsed_time, acceleration_length);
			float constant_velocity_interval= std::max(elapsed_time- acceleration_interval, 0.0f);

			float elapsed_distance= (acceleration_interval* (acceleration_interval/ acceleration_length)/ 2.0f+ constant_velocity_interval* 1.0f)/ GetLength();
			float moment= elapsed_distance- (int)elapsed_distance;

			float velocity;
			if(elapsed_time> acceleration_length)
				velocity= 1.0f;
			else
				velocity= acceleration_interval/ acceleration_length;

			SetLastMoment(moment, velocity);
			return moment;
		}
	}
}
