#include "GameSystem.h"
#include "Action.h"

void GameSystem::PruneActions()
{
	for(unsigned int i= 0; i< actions.size(); i++)
	{
		if(actions[i]->HasFinished())
		{
			delete actions[i];
			actions.erase(actions.begin()+ i);
			i--;
		}
	}
}

void GameSystem::Initialize()
{

}

void GameSystem::Terminate()
{
}

void GameSystem::SubmitAction(Action *action)
{
	actions.push_back(action);
}

void GameSystem::Step(Chronons chronons)
{
	if(sequential_mode)
	{
		if(actions.size()> 0)
			actions[0]->Step(chronons);
	}
	else
		for(Action *action : actions)
			action->Step(chronons);
	PruneActions();

	space.Step(chronons);
}

void GameSystem::Save(string filename)
{
	space.Save(filename);
}

void GameSystem::Load(string filename)
{
	space.Load(filename);
}
