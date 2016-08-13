#ifndef EBR_TIMER
#define EBR_TIMER

class Timer
{
	unsigned int elapsed_milliseconds;
	int last_ticks;

	bool is_paused= false;
	bool is_stopped= true;

public:
	void Start();
	float Pause();
	float Stop();

	float GetElapsedSeconds();
};

#endif