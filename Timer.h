#ifndef EBR_TIMER
#define EBR_TIMER

class Timer
{
	unsigned int elapsed_milliseconds= 0;
	int last_ticks;

	bool is_paused= false;
	bool is_stopped= true;

public:
	void Start();
	float Pause();
	float Stop();

	float GetElapsedSeconds();

	static Timer get_rays_timer;
	static Timer embree_timer;
	static Timer shading_timer;
	static Timer develop_timer;
	static Timer pre_shading_timer;
};

#endif