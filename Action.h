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
};

#endif