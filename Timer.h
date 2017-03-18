#ifndef EBR_TIMER
#define EBR_TIMER

//All returned timings are in seconds
class Timer
{
	unsigned int elapsed_milliseconds= 0;
	int last_ticks;

	bool is_paused= false;
	bool is_stopped= true;

	void AddTime();

public:
	void Start();
	float Pause();
	float Stop();

	unsigned int GetElapsedMilliseconds();
	float GetElapsedSeconds();

	static Timer get_rays_timer;
	static Timer embree_timer;
	static Timer primary_shading_timer;
	static Timer secondary_shading_timer;
	static Timer develop_timer;
	static Timer pre_shading_timer;
	static Timer shadow_timer;
};

#endif