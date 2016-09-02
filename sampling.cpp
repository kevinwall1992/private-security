#include "Sampling.h"

/*float GetRandomFloat()
{
	float random= (rand()* (RAND_MAX+ 1)+ rand())/ (float)(RAND_MAX* (RAND_MAX+ 1));

	//Needed because one day I got 1.00003 or something...
	if(random> 1)
		random= 1.0f;

	return random;
}*/

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

/*Vector2f SampleUnitDisc(std::atomic<int> &index)
{
	Vector2f sample;

	do sample= Geometry::Make::Vector2f_(HaltonSequence(2, index)* 2- 1, HaltonSequence(3, index++)* 2- 1);
	while(sample.Magnitude()> 1);

	return sample;
}

Vector3f SampleUnitSphere(std::atomic<int> &index)
{
	Vector3f sample;

	do sample= Geometry::Make::Vector3f_(HaltonSequence(2, index)* 2- 1, HaltonSequence(3, index)* 2- 1, HaltonSequence(5, index++)* 2- 1);
	while(sample.Magnitude()> 1);

	return sample;
}

Vector3f SampleHemisphere_Uniform(Vector3f normal, std::atomic<int> &index)
{
	Vector3f sample;

	do sample= SampleUnitSphere(index);
	while(sample.Dot(normal)< 0);

	return sample.GetNormalized();

}

Vector3f SampleHemisphere_GeometryImportance(Vector3f normal, std::atomic<int> &index)
{
	Vector2f uv= SampleUnitDisc(index);
	float z= sqrt(1- pow(uv.Magnitude(), 2));

	Vector3f u_axis= Geometry::Cross(normal, Geometry::Make::Vector3f_(1, 0, 0));
	if(u_axis.Magnitude()< 0.1f)
		u_axis= Geometry::Cross(normal, Geometry::Make::Vector3f_(0, 1, 0));

	u_axis= u_axis.GetNormalized();
	Vector3f v_axis= Geometry::Cross(normal, u_axis);

	Vector3f xy= Geometry::Unflatten(Vector3f(), u_axis, v_axis, Geometry::Make::Vector2f_(uv[Axis::X], uv[Axis::Y]));

	return xy+ normal* z;
}

LayerImportanceSampler::LayerImportanceSampler(vector<float> layer_thicknesses)
{
	total= 0;

	for(unsigned int i= 0; i< layer_thicknesses.size(); i++)
	{
		total+= layer_thicknesses[i];
		layers.push_back(total);
	}
}

LayerImportanceSampler::LayerImportanceSampler()
{
	total= -1;
}

int LayerImportanceSampler::Sample() const
{
	float random_float= GetRandomFloat()* total;

	for(unsigned int i= 0; i< layers.size(); i++)
	{
		if(random_float<= layers[i])
			return i;
	}

	assert(("Layer sampler is broke", false));

	return -1;
}

float LayerImportanceSampler::GetTotal() const
{
	return total;
}*/