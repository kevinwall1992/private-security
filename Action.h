#ifndef PS_ACTION
#define PS_ACTION

#include "Chronon.h"
#include "Actor.h"
#include "Property.h"

class Action : public Chronal
{
protected:
	Actor *actor;

public:
	Action(Actor *actor);

	virtual bool HasFinished()= 0;

	//This allows Action to control which animation will play while it executes.
	//Obviously, this is a very rough implementation; in future I'm
	//thinking we may have an interface or something
	virtual string GetName()= 0;
	virtual float GetProgress()= 0;
	virtual float GetDimensionalProgress();
};

class IntervalAction : public Action
{
	bool has_started= false;
	bool has_finished= false;

	Chronons duration;
	Chronons time_spent= 0;

protected:
	virtual void Start();
	virtual void Proceed(float percentage_done);
	virtual void Finish();

public:
	IntervalAction(Actor *actor, Chronons duration);

	Chronons GetDuration();
	Chronons GetTimeLeft();

	void Step(Chronons chronons);

	bool HasFinished();

	virtual float GetProgress();
};

#endif