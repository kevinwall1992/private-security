#ifndef PS_CYCLE
#define PS_CYCLE

class Cycle
{
	float length;

	float last_moment= 0.0f;
	float last_velocity= 0.0f;

protected:
	float GetLength();
	float GetLastMoment();
	float GetLastVelocity();

	void SetLastMoment(float moment, float velocity);

	virtual float GetElapsedTime(unsigned int ticks);
	virtual float GetElapsedCycles(unsigned int ticks);
	virtual float GetMoment(unsigned int ticks);

	float ElapsedTimeToElapsedCycles(float elapsed_time);
	float ElapsedTimeToMoment(float elapsed_time);
	float ElapsedCyclesToMoment(float elapsed_cycles);

	virtual void Reset();

public:
	Cycle(float length);

	virtual float GetMoment();

	unsigned int SDL_GetTicks_();
};

class StartStopCycle : public Cycle
{
	int start_ticks= -1;
	bool is_capped= false;
	bool is_stopping= false;
	bool is_paused= false;

	float pause_moment= 0.0f;
	float pause_elapsed_time= 0.0f;
	float stop_moment= 0.0f;
	float stop_elapsed_time= 0.0f;

protected:
	StartStopCycle(float length, int start_ticks, bool is_capped, float pause_moment, float pause_elapsed_time);

	virtual float GetElapsedTime(unsigned int ticks);
	virtual float GetElapsedCycles(unsigned int ticks);

	int GetStartTicks();
	bool IsCapped();
	bool IsStopping();
	bool IsPaused();

	float GetPauseMoment();
	float GetPauseElapsedTime();
	float GetPauseElapsedCycles();
	float GetStopMoment();
	float GetStopElapsedTime();
	float GetStopElapsedCycles();

	virtual void Reset();

public:
	StartStopCycle(float length, bool cap= false);

	virtual void Start();
	virtual void Stop();
	virtual void Pause();

	virtual float GetMoment();
};

class SmoothCycle : public StartStopCycle
{
	float acceleration_length= 0.5f;

	float stop_velocity= 0.0f;
	float pause_velocity= 0.0f;

	SmoothCycle *sub_cycle= nullptr;
	bool is_sub_cycle= false;

protected:
	SmoothCycle(float length, int start_ticks, bool is_capped, float pause_moment, float pause_elapsed_time, float pause_velocity);

	virtual void Reset();

public:
	SmoothCycle(float length, float acceleration_length);
	SmoothCycle(float length, bool cap= true);

	virtual void Start();
	virtual void Stop();
	virtual void Pause();

	virtual float GetMoment();
};

#endif