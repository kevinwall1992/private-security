#include "Light.h"


Light::Light()
{

}

bool Light::IsAmbient()
{
	return false;
}

bool Light::IsSoft()
{
	return false;
}

bool Light::IsGenerative()
{
	return false;
}

Vec3f Light::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return Vec3f(0, 0, 0);
}


AmbientLight::AmbientLight(Color intensity_)
{
	intensity= intensity_;
}

bool AmbientLight::IsAmbient()
{
	return true;
}

Color AmbientLight::GetIntensity()
{
	return intensity;
}

Vec3f AmbientLight::GetPosition()
{
	return Vec3f(0, 0, 0);
}

Color AmbientLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

PointLight::PointLight(Vec3f position_, Color intensity_)
{
	position= position_;
	intensity= intensity_;
}

PointLight::PointLight()
{
	
}

bool PointLight::IsGenerative()
{
	return true;
}

Color PointLight::GetIntensity()
{
	return intensity;
}

Vec3f PointLight::GetPosition()
{
	return position;
}

Color PointLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

Vec3f PointLight::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return point- position;
}

/*
DiscLight::DiscLight(Vec3f position, Color intensity, float radius_)
	: PointLight(position, intensity)
{
	radius= radius_;
}

bool DiscLight::IsSoft()
{
	return true;
}

Color DiscLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

Vec3f DiscLight::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return Vec3f(0, 0, 0);
}
*/