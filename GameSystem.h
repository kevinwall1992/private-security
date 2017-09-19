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
	Space space= Space("data/content/maps/test_map.xml");

	void SubmitAction(Action *action);

	void Step(Chronons chronons);

	void Save(string filename);
	void Load(string filename);

	friend System;
};

#endif