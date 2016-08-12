#ifndef EBR_LIGHT
#define EBR_LIGHT

#include "Color.h"
#include "Math.h"


class Light
{
public:
	Light();
	
	virtual bool IsAmbient();
	virtual bool IsSoft();
	virtual bool IsGenerative();

	virtual Color GetLuminosity(Vec3f point)= 0;
	virtual Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
	
	//virtual Ray SampleLightRay(int sample_index);//put this in GenerativeLight?
};

class AmbientLight : public Light
{
	Color intensity;

public:
	AmbientLight(Color intensity);

	bool IsAmbient();
	Color GetLuminosity(Vec3f point);
}; 

class PointLight : public Light
{
protected:
	Vec3f position;
	Color intensity;

public:
	PointLight(Vec3f position, Color intensity);

	bool IsGenerative();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};

class DiscLight : public PointLight
{
	float radius;

public:
	DiscLight(Vec3f position, Color intensity, float radius);

	bool IsSoft();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};

#endif