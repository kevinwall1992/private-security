#ifndef CRT_SAMPLING
#define CRT_SAMPLING

#ifndef ISPC_CODE
#include "Common.h"
#include "Math.h"

float HaltonSequence(int base, int sample_index);
Vec2f SampleUnitSquare(int sample_index);

Vec2f SampleUnitDisc(int &index);

#else

#include "Parameters.h"

#if BAKE_DISC_SAMPLES == 0
//http://en.wikipedia.org/wiki/Halton_sequence
inline float HaltonSequence(uniform int base, int i)
{
	float result= 0;
	float f= 1.0f/ base;
	while(i> 0)//try cwhile
	{
		result= result+ f* (i% base);
		i= i/ base;
		f= f/ base;
	}
		
	return result;
}

inline uniform float HaltonSequence(uniform int base, uniform int i)
{
	uniform float result= 0;
	uniform float f= 1.0f/ base;
	while(i> 0)//try cwhile
	{
		result= result+ f* (i% base);
		i= i/ base;
		f= f/ base;
	}
		
	return result;
}

inline void SampleUnitDisc(uniform int * index, varying float * uniform x, varying float * uniform y, varying float * uniform magnitude_squared)
{
	//could try cdo, doesn't hurt...
	do
	{
		//uniform int index__= (*index)++;
	    //int index_= index__* programCount+ programIndex;
		int index_= atomic_add_local(index, 1);
		//print("%\n", index_);

		*x= HaltonSequence(2, index_)* 2- 1;
		*y= HaltonSequence(3, index_)* 2- 1;

		*magnitude_squared= *x* *x+ *y* *y;
	}
	while(*magnitude_squared> 1);
}
#endif

void SampleHemisphere_GeometryImportance(varying int index, 
										 varying float normal_x, varying float normal_y, varying float normal_z, 
										 varying float * uniform sample_x, varying float * uniform sample_y, varying float * uniform sample_z,
										 uniform float disc_samples[])
{
#if BAKE_DISC_SAMPLES
	float u= disc_samples[index* PACKET_SIZE* 2+ programIndex+ 0];
	float v= disc_samples[index* PACKET_SIZE* 2+ programIndex+ PACKET_SIZE];
	//float u= disc_samples[index* 2+ 0];
	//float v= disc_samples[index* 2+ 1];

	float uv_magnitude_squared= u* u+ v* v;

#else
	float u, v;
	float uv_magnitude_squared;
	SampleUnitDisc(index, &u, &v, &uv_magnitude_squared);
#endif

	float w= sqrt(1- uv_magnitude_squared);

	float u_x= 0;
	float u_y= normal_z;
	float u_z= -normal_y;
	float u_magnitude_squared= u_y* u_y+ u_z* u_z;
	cif(u_magnitude_squared< 0.01f)
	{
		u_x= -normal_z;
		u_y= 0;
		u_z= normal_x;

		float u_magnitude= sqrt(u_x* u_x+ u_z* u_z);
		u_x/= u_magnitude;
		u_z/= u_magnitude;

		float v_x= normal_y* u_z;
		float v_y= normal_z* u_x- normal_x* u_z;
		float v_z= -normal_y* u_x;

		*sample_x= u* u_x+ v* v_x+ w* normal_x;
		*sample_y=		   v* v_y+ w* normal_y;
		*sample_z= u* u_z+ v* v_z+ w* normal_z;
		return;
	}
	float u_magnitude= sqrt(u_magnitude_squared);
	u_y/= u_magnitude;
	u_z/= u_magnitude;

	float v_x= normal_y* u_z- normal_z* u_y;
	float v_y= -normal_x* u_z;
	float v_z= normal_x* u_y;

	*sample_x=		   v* v_x+ w* normal_x;
	*sample_y= u* u_y+ v* v_y+ w* normal_y;
	*sample_z= u* u_z+ v* v_z+ w* normal_z;
}

/*

void SampleHemisphere_GeometryImportance(uniform int * uniform index, 
										 varying float normal_x, varying float normal_y, varying float normal_z, 
										 varying float * uniform sample_x, varying float * uniform sample_y, varying float * uniform sample_z)
{
	float u, v;
	float uv_magnitude_squared;
	SampleUnitDisc(index, &u, &v, &uv_magnitude_squared);

	float w= sqrt(1- uv_magnitude_squared);

	float u_x= 0;
	float u_y= normal_z;
	float u_z= -normal_y;
	float u_magnitude_squared= u_y* u_y+ u_z* u_z;
	cif(u_magnitude_squared< 0.01f)
	{
		u_x= -normal_z;
		u_y= 0;
		u_z= normal_x;

		u_magnitude_squared= u_x* u_x+ u_z* u_z;

		u_x/= u_magnitude_squared;
		u_z/= u_magnitude_squared;

		float v_x= normal_y* u_z;
		float v_y= normal_z* u_x- normal_x* u_z;
		float v_z= -normal_y* u_x;

		*sample_x= u* u_x+ v* v_x+ w* normal_x;
		*sample_y=		   v* v_y+ w* normal_y;
		*sample_z= u* u_z+ v* v_z+ w* normal_z;
	}
	u_y/= u_magnitude_squared;
	u_z/= u_magnitude_squared;

	float v_x= normal_y* u_z- normal_z* u_y;
	float v_y= -normal_x* u_z;
	float v_z= normal_x* u_y;

	*sample_x=		   v* v_x+ w* normal_x;
	*sample_y= u* u_y+ v* v_y+ w* normal_y;
	*sample_z= u* u_z+ v* v_z+ w* normal_z;
}

*/

#endif



#endif