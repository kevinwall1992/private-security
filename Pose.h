#ifndef PS_POSE
#define PS_POSE

#include "Chronon.h"


class Pose : public Chronal
{
public:
	enum State { Standing, Walking };

private:
	State state;
	Chronons moment;

public:
	State GetState();
	void SetState(State state);

	Chronons GetMoment();

	void Step(Chronons chronons);
};

class HasPose : public virtual Chronal
{
public:
	Pose pose;

	void Step(Chronons chronons);
};

#endif
