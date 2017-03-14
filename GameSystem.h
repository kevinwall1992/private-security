#ifndef PS_GAME_SYSTEM
#define PS_GAME_SYSTEM

#include "System.h"
#include "Space.h"
#include "Chronon.h"

class Action;

class GameSystem : public System, public Chronal
{
	bool sequential_mode= true;

	vector<Action *> actions;

	void PruneActions();

protected:
	void Initialize();
	void Terminate();

public:
	Space space= Space(10, 10, 3);

	void SubmitAction(Action *action);

	void Step(Chronons chronons);

	friend System;
};

#endif