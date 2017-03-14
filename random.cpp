#include "Random.h"
#include "Math.h"
#include "Common.h"

RandomIterator::RandomIterator(int prime_, int seed_primitive_root, int count_, int seed_state)
{
	prime= prime_;
	count= count_;
	state= seed_state;
    
	vector<int> primitive_roots;
	primitive_roots.push_back(seed_primitive_root);
    
	int max_exponent= (int)(log(std::numeric_limits<int>::max())/ log(seed_primitive_root));
	for(int i= 0; i< max_exponent; i++)
	{
		int congruent= Math::Pow(seed_primitive_root, i)% prime;

		bool already_present= false;
		for(unsigned j= 0; j< primitive_roots.size(); j++)
			if(primitive_roots[j]== congruent)
				already_present= true;
		if(already_present)
			continue;

		if(Math::GCD(i, prime- 1) == 1)
			primitive_roots.push_back(congruent);
	}
      
	primitive_roots.resize(primitive_roots.size()/ 2+ 1);
	for(int i= (int)primitive_roots.size()- 1; i>= 0; i--)
	{
		if(primitive_roots[i]< (std::numeric_limits<int>::max()/ prime))
		{
			primitive_root= primitive_roots[i];
			if(state< 1)
				state= primitive_root;
			return;
		}
	}

	assert(primitive_roots.size()> 0 && "RandomIterator(): primitive seed too large.");
}

int RandomIterator::GetNext()
{
	do state= (state* primitive_root)% prime;
	while(state> count);

	return state- 1;
}

void RandomIterator::Reset(int seed_state)
{
	state= seed_state;
}

void RandomIterator::Reset()
{
	state= primitive_root;
}