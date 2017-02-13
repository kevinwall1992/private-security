#ifndef PS_PROPERTY
#define PS_PROPERTY

#include "Mutable.h"

#include <assert.h>


template<class T>
class ImmutableProperty
{

protected:
	T *value;

	virtual T & operator=(const ImmutableProperty<T> &other)
	{
		assert(false && "ImmutableProperty::operator=: Dont call this.");

		return *value;
	}

public:
	ImmutableProperty(T *value)
	{
		this->value= value;
	}

	T Value() const
	{
		return *value;
	}

	virtual operator T() const
	{
		return Value();
	}
};

template<class T>
class Property : public ImmutableProperty<T>, public Sensor
{
	T *value;

public:
	Property(T *value, Mutable *parent= nullptr)
		: ImmutableProperty(value), Sensor(parent)
	{
		this->value= value;
	}

	virtual T & operator=(const T &other)
	{
		*value= other;
		Touch();

		return *value;
	}

	virtual T & operator=(const Property<T> &other)
	{
		*value= *(other.value);
		Touch();

		return *value;
	}

	virtual T & operator+=(const T &other)
	{
		*value+= other;
		Touch();

		return *value;
	}
};

typedef Property<int> IntProperty;
typedef Property<float> FloatProperty;

#endif