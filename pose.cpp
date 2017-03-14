#include "Pose.h"

Pose::State Pose::GetState()
{
	return state;
}

void Pose::SetState(State state_)
{
	state= state_;
	moment= 0;
}

Chronons Pose::GetMoment()
{
	return moment;
}

void Pose::Step(Chronons chronons)
{
	moment+= chronons;
}

void HasPose::Step(Chronons chronons)
{
	pose.Step(chronons);
}
