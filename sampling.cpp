#include "Sampling.h"

//http://en.wikipedia.org/wiki/Halton_sequence
float HaltonSequence(int base, int i)
{
	float result= 0;
	float f = 1.0f;
	while(i> 0) 
	{
		f= f/ base;
		result+= f* (i% base);
		i= i/ base;
	}
		
	return result;
}

Vec2f SampleUnitSquare(int sample_index)
{
	return Vec2f(HaltonSequence(2, sample_index), HaltonSequence(3, sample_index));
}

Vec2f SampleUnitDisc(int &index)
{
	Vec2f sample;

	do sample= Vec2f(HaltonSequence(2, index)* 2- 1, HaltonSequence(3, index++)* 2- 1);
	while(sqrt(sample.x* sample.x+ sample.y* sample.y)> 1);

	return sample;
}

