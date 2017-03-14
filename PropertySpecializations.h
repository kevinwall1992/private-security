#ifndef PS_PROPERTY_SPECIALIZATIONS
#define PS_PROPERTY_SPECIALIZATIONS

#include "Property.h"
#include "Vector.h"


//Make this simpler with inheritance

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

	virtual Vec3f & operator*=(const Vec3f &other)
	{
		*value*= other;
		Touch();

		return *value;
	}

	Vec3f V()
	{
		return *value;
	}
};

typedef Property<Vec3f> Vec3fProperty;


template<>
class Property<Vec3i> : public Sensor
{
	Vec3i *value;

public:
	IntProperty x, y, z;

	Property(Vec3i *value, Mutable *parent= nullptr)
		: Sensor(parent), x(value->c+ 0), y(value->c+ 1), z(value->c+ 2)
	{
		this->value= value;
	}

	virtual Vec3i & operator=(const Vec3i &other)
	{
		*value= other;
		Touch();

		return *value;
	}

	virtual Vec3i & operator=(const Property<Vec3i> &other)
	{
		*value= *(other.value);
		Touch();

		return *value;
	}

	virtual operator Vec3i()
	{
		return *value;
	}

	virtual Vec3i & operator+=(const Vec3i &other)
	{
		*value+= other;
		Touch();

		return *value;
	}

	virtual Vec3i & operator*=(const Vec3i &other)
	{
		*value*= other;
		Touch();

		return *value;
	}

	Vec3i V()
	{
		return *value;
	}
};

typedef Property<Vec3i> Vec3iProperty;


template<>
class Property<Vec2i> : public Sensor
{
	Vec2i *value;

public:
	IntProperty x, y;

	Property(Vec2i *value, Mutable *parent= nullptr)
		: Sensor(parent), x(value->c+ 0), y(value->c+ 1)
	{
		this->value= value;
	}

	virtual Vec2i & operator=(const Vec2i &other)
	{
		*value= other;
		Touch();

		return *value;
	}

	virtual Vec2i & operator=(const Property<Vec2i> &other)
	{
		*value= *(other.value);
		Touch();

		return *value;
	}

	virtual operator Vec2i()
	{
		return *value;
	}

	virtual Vec2i & operator+=(const Vec2i &other)
	{
		*value+= other;
		Touch();

		return *value;
	}

	virtual Vec2i & operator*=(const Vec2i &other)
	{
		*value*= other;
		Touch();

		return *value;
	}

	Vec2i V()
	{
		return *value;
	}
};

typedef Property<Vec2i> Vec2iProperty;


template<>
class Property<Vec2f> : public Sensor
{
	Vec2f *value;

public:
	FloatProperty x, y;

	Property(Vec2f *value, Mutable *parent= nullptr)
		: Sensor(parent), x(value->c+ 0), y(value->c+ 1)
	{
		this->value= value;
	}

	virtual Vec2f & operator=(const Vec2f &other)
	{
		*value= other;
		Touch();

		return *value;
	}

	virtual Vec2f & operator=(const Property<Vec2f> &other)
	{
		*value= *(other.value);
		Touch();

		return *value;
	}

	virtual operator Vec2f()
	{
		return *value;
	}

	virtual Vec2f & operator+=(const Vec2f &other)
	{
		*value+= other;
		Touch();

		return *value;
	}

	virtual Vec2f & operator*=(const Vec2f &other)
	{
		*value*= other;
		Touch();

		return *value;
	}

	Vec2f V()
	{
		return *value;
	}
};

typedef Property<Vec2f> Vec2fProperty;

#endif