#include "Threading.h"

Barrier::Barrier(int size_)
{
	size= size_;
	waiting_count= 0;
	condition= true;
}

void Barrier::Wait()
{
	std::unique_lock<std::mutex> unique_lock(mutex);

	int index= ++waiting_count;
	if(index== size)
	{
		waiting_count= 0;
		condition= !condition;
		condition_variable.notify_all();
	}
	else if(condition)
		condition_variable.wait(unique_lock, [this] { return !condition; });
	else
		condition_variable.wait(unique_lock, [this] { return condition; });
}


Turn::Turn(std::mutex *mutex_)
{
	mutex= mutex_;
	mutex->lock();
}

void Turn::AllDone()
{
	mutex->unlock();
}

namespace Threading
{
	ItemBase::ItemBase(ItemName name)
	{
		this->name= name;
	}

	bool ItemBase::IsNamed(ItemName &name)
	{
		return strcmp(this->name, name)== 0;
	}
}


vector<Team *> Team::teams;
Team Team::serial_team= Team();

__declspec(thread) Team *my_team;
__declspec(thread) int my_index;

Team::Team()
	: barrier(1)
{
	SetTeamSize(1);
}

void Team::SetTeamSize(int team_size)
{
	members.resize(team_size);
	buffers.resize(team_size);
	objects.resize(team_size);
}

int Team::GetMyIndex()
{
	if(my_team== nullptr)
		GetMyTeam();

	if(this!= my_team)
		return -1;

	return my_index;
}

int Team::GetTeamSize()
{
	return (int)members.size();
}

bool Team::RollcallIsFinished()
{
	return rollcall_is_finished;
}

Team * Team::GetMyTeam()
{
	if(my_team!= nullptr)
		return my_team;

	std::thread::id my_id= std::this_thread::get_id();
	for(unsigned int i= 0; i< teams.size(); i++)
	{
		Team *team= teams[i];

		std::unique_lock<std::mutex> unique_lock(team->rollcall_mutex);
		teams[i]->rollcall_cv.wait(unique_lock, [&] 
		{ 
			return team->RollcallIsFinished();
		});

		for(unsigned int j= 0; j< teams[i]->members.size(); j++)
			if(teams[i]->members[j].get_id()== my_id)
			{
				my_team= teams[i];
				my_index= j;
				return my_team;
			}
	}

	my_team= &serial_team;
	my_index= 0;
	return my_team;
}

Team::Team(int team_size)
	: barrier(team_size)
{
	SetTeamSize(team_size);

	teams.push_back(this);
}

Team::~Team()
{
	for(unsigned int i= 0; i< teams.size(); i++)
	{
		if(teams[i]== this)
		{
			teams.erase(teams.begin()+ i);
			break;
		}
	}
}

void Team::Reset()
{
	Iterator<Threading::Item<std::atomic_int> *> *iterator= counters.GetIterator();

	Threading::Item<std::atomic_int> *item;
	while((item= iterator->GetNext())!= nullptr)
		item->object= 0;

	delete iterator;
}

void Team::WaitForOthers()
{
	GetMyTeam()->barrier.Wait();
}

int Team::TakeANumber(Threading::ItemName counter_name)
{
	return GetMyTeam()->counters.GetElement(counter_name)->object++;
}

Turn Team::WaitForTurn(Threading::ItemName queue_name)
{
	return &(GetMyTeam()->queues.GetElement(queue_name)->object);
}
