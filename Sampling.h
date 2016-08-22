#ifndef CRT_SAMPLING
#define CRT_SAMPLING

#ifndef ISPC_CODE
#include "Common.h"
#include "Math.h"

float HaltonSequence(int base, int sample_index);
Vec2f SampleUnitSquare(int sample_index);

#else
	/*float HaltonSequence(uniform int base, int i)
	{
		float result= 0;
		float f = 1.0f/ base;
		while(i> 0) 
		{
			result= result+ f* (i% base);
			i= i/ base;
			f= i/ base;
		}
		
		return result;
	}*/

	uniform float HaltonSequence(uniform int base, uniform int i)
	{
		uniform float result= 0;
		uniform float f = 1.0f/ base;
		while(i> 0) 
		{
			result= result+ f* (i% base);
			i= i/ base;
			f= i/ base;
		}
		
		return result;
	}

	//Apparently scatter happens. If we end up wanting this, need to address that
	/*void SampleUnitSquare(uniform int sample_index, uniform float<2> *vector)
	{
		vector->x= HaltonSequence(2, sample_index);
		vector->y= HaltonSequence(3, sample_index);
	}*/

#endif



#endif