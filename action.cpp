#include "Action.h"


Action::Action(Actor *actor_)
{
	actor= actor_;
}

float Action::GetDimensionalProgress()
{
	return GetProgress();
}

void IntervalAction::Start()
{
	actor->SetAction(this);
}

void IntervalAction::Proceed(float percentage_done)
{
}

void IntervalAction::Finish()
{
	actor->SetAction(nullptr);
}

IntervalAction::IntervalAction(Actor *actor, Chronons duration_)
	: Action(actor)
{
	duration= duration_;
}

Chronons IntervalAction::GetDuration()
{
	return duration;
}

Chronons IntervalAction::GetTimeLeft()
{
	return duration- time_spent;
}

void IntervalAction::Step(Chronons chronons)
{
	if(!has_started)
	{
		Start();
		has_started= true;
	}

	time_spent+= chronons;
	Proceed(GetProgress());

	if(time_spent>= duration)
	{
		Finish();
		has_finished= true;
	}
}

bool IntervalAction::HasFinished()
{
	return has_finished;
}

float IntervalAction::GetProgress()
{
	return time_spent/ (float)duration;
}
