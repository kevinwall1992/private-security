#ifndef EBR_THREADING
#define EBR_THREADING

#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_map>
#include <deque>

#include "Common.h"
#include "Named.h"
#include "Catalog.h"

//http://stackoverflow.com/questions/24465533/implementing-boostbarrier-in-c11
class Barrier
{
private:
    std::mutex mutex;
    std::condition_variable condition_variable;
    std::atomic_int waiting_count;

	bool condition;
	int size;

public:
	explicit Barrier(int size);

    void Wait();
};


namespace Threading
{
	typedef const char * ItemName;

	class ItemBase : public Named<ItemName>
	{
		ItemName name;

	public:
		ItemBase(ItemName name);

		bool IsNamed(ItemName &name);
	};

	template<class T>
	struct Item : public ItemBase
	{
		T object;

		Item(ItemName name) : ItemBase(name) { }
	};

	template<>
	struct Item<std::atomic_int> : public ItemBase
	{
		std::atomic_int object;

		Item(ItemName name) : ItemBase(name) { object= 0; }
	};

	template<class T>
	struct Item<T *> : public ItemBase
	{
		T *object;

		Item(ItemName name) : ItemBase(name) { object= nullptr; }
	};

	template<class T>
	using ItemCatalog= Catalog<Item<T>, ItemName>;

	struct SizedBuffer
	{
		void *buffer;
		size_t size;

		SizedBuffer()
		{
			buffer= nullptr;
			size= -1;
		}
	};
}

class Turn
{
	std::mutex *mutex;

public:
	Turn(std::mutex *mutex);

	void AllDone();
};

class Team
{
	static vector<Team *> teams;
	static Team serial_team;

	vector<std::thread> members;
	bool rollcall_is_finished;
	std::condition_variable rollcall_cv;
	std::mutex rollcall_mutex;

	Threading::ItemCatalog<std::mutex> queues;
	Threading::ItemCatalog<std::atomic_int> counters, persistent_counters;
	std::deque<Threading::ItemCatalog<Threading::SizedBuffer>> buffers;
	std::deque<Threading::ItemCatalog<void *>> objects;
	std::deque<Threading::ItemCatalog<bool>> markers;

	Barrier barrier;


	Team();

	void SetTeamSize(int team_size);

	int GetTeamSize();
	int GetMyIndex();

	bool RollcallIsFinished();

public:
	Team(int team_size);
	~Team();

	void Reset();
	
	//Eventually would like variadic template
	template<class Function, class Class, class Argument>
	void Assign(Function f, Class *instance, Argument argument)
	{
		Reset();

		if(this== &serial_team)
		{
			(*instance.*f)(argument);
			return;
		}

		rollcall_is_finished= false;
		for(unsigned int i= 0; i< members.size(); i++)
			members[i]= std::thread(f, instance, argument);

		rollcall_mutex.lock();
		rollcall_is_finished= true;
		rollcall_cv.notify_all();
		rollcall_mutex.unlock();

		for(unsigned int i= 0; i< members.size(); i++)
			members[i].join();
	}

	template<class Function, class Class, class Argument0, class Argument1>
	void Assign(Function f, Class *instance, Argument0 argument0, Argument1 argument1);
	template<class Function, class Argument>
	void Assign(Function f, Argument argument);
	template<class Function, class Argument0, class Argument1>
	void Assign(Function f, Argument0 argument0, Argument1 argument1);


	static Team * GetMyTeam();

	template<class T>
	static T * GetBuffer(Threading::ItemName buffer_name, int max_element_count)
	{
		Team *team= GetMyTeam();

		Threading::SizedBuffer *buffer= &(team->buffers[team->GetMyIndex()].GetElement(buffer_name)->object);
		size_t size= max_element_count* sizeof(T);
		
		if(buffer->size< size && buffer->buffer!= nullptr)
		{
			free(buffer->buffer);
			buffer->buffer= nullptr;
		}

		if(buffer->buffer== nullptr)
		{
			buffer->buffer= malloc(size);
			buffer->size= size;

			((void **)buffer->buffer)[0]= 0;
		}

		return (T *)(buffer->buffer);
	}

	template<class T>
	static T * SetObject(Threading::ItemName object_name, T *object)
	{
		Team *team= GetMyTeam();

		team->objects[team->GetMyIndex()].GetElement(object_name)->object= object;

		return object;
	}

	template<class T>
	static T * GetObject(Threading::ItemName object_name)
	{
		Team *team= GetMyTeam();

		return (T *)(team->objects[team->GetMyIndex()].GetElement(object_name)->object);
	}

	template<class T>
	static T * GetObject_Instanciate(Threading::ItemName object_name)
	{
		T *object= GetObject<T>(object_name);
		if(object== nullptr)
			object= SetObject<T>(object_name, new T());

		return object;
	}


	static void WaitForOthers();
	static int TakeANumber(Threading::ItemName counter_name);
	static int TakeANumber_Persistent(Threading::ItemName counter_name);
	static Turn WaitForTurn(Threading::ItemName queue_name);
	static bool FirstTime(Threading::ItemName marker_name);
};

#endif