#ifndef PS_GARBAGE_CAN
#define PS_GARBAGE_CAN

#include <vector>

using std::vector;


class GarbageCanBase
{
	static vector<GarbageCanBase *> garbage_cans;

protected:
	virtual void Collect()= 0;

public:
	GarbageCanBase();

	static void CollectAll();
};

template<class T>
class GarbageCan : public GarbageCanBase
{
	static GarbageCan<T> *this_garbage_can;

	vector<T *> garbages;

protected:
	void Put(T *garbage)
	{
		garbages.push_back(garbage);
	}

	void Collect()
	{
		for(T *garbage : garbages)
			delete garbage;

		garbages.clear();
	}

public:
	static void ThrowAway(T *garbage)
	{
		this_garbage_can->Put(garbage);
	}
};

template<class T>
GarbageCan<T> * GarbageCan<T>::this_garbage_can= new GarbageCan<T>();

class Landfill
{
public:
	static void Dump();
};

#endif
