#ifndef PS_PROPERTY
#define PS_PROPERTY

#include "Promote.h"

#include <assert.h>


template<class T>
class PropertyBase
{
public:
	virtual T Get() const= 0;
	virtual void Set(const T &value)= 0;

	virtual operator T() const
	{
		return Get();
	}

	virtual T operator=(const T &other)
	{
		Set(other);
		
		return Get();
	}

	virtual T operator=(const PropertyBase<T> &other)
	{
		Set(other.Get());

		return Get();
	}

	virtual bool operator==(const T &other)
	{
		return Get()== other;
	}

	virtual bool operator==(const PropertyBase<T> &other)
	{
		return Get()== other.Get();
	}

	template<class U>
	typename Utility::Promote<T, U>::type operator+(const U &other)
	{
		return Get()+ other;
	}

	template<class U>
	typename Utility::Promote<T, U>::type operator-(const U &other)
	{
		return Get()- other;
	}

	template<class U>
	typename Utility::Promote<T, U>::type operator*(const U &other)
	{
		return Get()* other;
	}

	template<class U>
	typename Utility::Promote<T, U>::type operator/(const U &other)
	{
		return Get()/ other;
	}

	template<class U>
	T operator+=(const U &other)
	{
		Set((T)(Get()+ other));

		return Get();
	}

	template<class U>
	T operator-=(const U &other)
	{
		Set((T)(Get()- other));

		return Get();
	}

	template<class U>
	T operator*=(const U &other)
	{
		Set((T)(Get()* other));

		return Get();
	}

	template<class U>
	T operator/=(const U &other)
	{
		Set((T)(Get()/ other));

		return Get();
	}
};


template<class T>
class Property : public PropertyBase<T>
{
	T *value_pointer;

	Property(const Property<T> &other)
	{
		assert(false && "Property::Property: Dont use copy constructor");
	}

public:
	Property(T *value_pointer)
	{
		this->value_pointer= value_pointer;
	}

	using PropertyBase::operator=;

	virtual T Get() const
	{
		return *value_pointer;
	}

	virtual void Set(const T &value)
	{
		*value_pointer= value;
	}
};

typedef Property<int> IntProperty;
typedef Property<float> FloatProperty;
typedef Property<float> BoolProperty;


template<class T>
class Getter : private Property<T>
{
	void Set(const T &value)
	{
		assert(false && "Getter::Set: Not supposed to call this");
	}

public:
	Getter(T *value_pointer)
		: Property(value_pointer)
	{
		
	}

	virtual operator T() const
	{
		return Get();
	}

	using Property<T>::operator==;
	using Property<T>::operator+;
	using Property<T>::operator-;
	using Property<T>::operator*;
	using Property<T>::operator/;

	using Property<T>::Get;
};

typedef Getter<int> IntGetter;
typedef Getter<float> FloatGetter;
typedef Getter<float> BoolGetter;

#endif