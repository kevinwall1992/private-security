#ifndef PS_ACTIONS
#define PS_ACTIONS

#include "Action.h"
#include "Movement.h"

class MoveAction : public IntervalAction
{
	Move *move;

protected:
	virtual void Start();
	void Proceed(float percentage_done);
	virtual void Finish();

public:
	MoveAction(Actor *actor, Move *move);

	virtual string GetName();
	virtual float GetDimensionalProgress();
};

#endif