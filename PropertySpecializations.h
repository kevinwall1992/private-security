#ifndef PS_PROPERTY_SPECIALIZATIONS
#define PS_PROPERTY_SPECIALIZATIONS

#include "Property.h"
#include "Vector.h"


template<>
class Property<Vec3f> : public Sensor
{
	Vec3f *value;

public:
	FloatProperty x, y, z;

	Property(Vec3f *value, Mutable *parent= nullptr)
		: Sensor(parent), x(value->c+ 0), y(value->c+ 1), z(value->c+ 2)
	{
		this->value= value;
	}

	virtual Vec3f & operator=(const Vec3f &other)
	{
		*value= other;
		Touch();

		return *value;
	}

	virtual Vec3f & operator=(const Property<Vec3f> &other)
	{
		*value= *(other.value);
		Touch();

		return *value;
	}

	virtual operator Vec3f()
	{
		return *value;
	}

	virtual Vec3f & operator+=(const Vec3f &other)
	{
		*value+= other;
		Touch();

		return *value;
	}

	Vec3f V()
	{
		return *value;
	}
};

typedef Property<Vec3f> Vec3fProperty;

#endif