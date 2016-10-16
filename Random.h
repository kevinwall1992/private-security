#ifndef EBR_RANDOM
#define EBR_RANDOM

#include "Container.h"

//Iterates over range [0, count) in random order
class RandomIterator : public Iterator<int>
{
	int prime;
	int primitive_root;
	int state;

	int count;

public:
	RandomIterator(int prime, int seed_primitive_root, int count, int seed_state= 0);

	int GetNext();

	void Reset(int seed_state);
	void Reset();
};

#endif