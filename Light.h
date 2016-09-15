#ifndef EBR_LIGHT
#define EBR_LIGHT

#include "Color.h"
#include "Math.h"
#include "ISPCKernels.h"

class Light
{
public:
	Light();
	
	virtual bool IsAmbient();
	virtual bool IsSoft();
	virtual bool IsGenerative();

	//Temporary hacks
	//Possible real solution would be special structure for kernel shading inside Scene or something
	virtual Color GetIntensity()= 0;
	virtual Vec3f GetPosition()= 0;

	virtual Color GetLuminosity(Vec3f point)= 0;
	virtual Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index= 0);
	
	//virtual Ray SampleLightRay(int sample_index);//put this in GenerativeLight?
};

class AmbientLight : public Light
{
	Color intensity;

public:
	AmbientLight(Color intensity);

	bool IsAmbient();

	Color GetIntensity();
	Vec3f GetPosition();

	Color GetLuminosity(Vec3f point);
}; 

class PointLight : public Light
{
protected:
	Vec3f position;
	Color intensity;

public:
	PointLight(Vec3f position, Color intensity);
	PointLight();

	bool IsGenerative();

	Color GetIntensity();
	Vec3f GetPosition();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};

/*class DiscLight : public PointLight
{
	float radius;

public:
	DiscLight(Vec3f position, Color intensity, float radius);

	bool IsSoft();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};*/

#endif