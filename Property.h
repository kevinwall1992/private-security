#ifndef PS_PROPERTY
#define PS_PROPERTY

#include "Promote.h"

#include <assert.h>


template<class T>
class PropertyBase
{
public:
	PropertyBase(const PropertyBase &other)
	{
		operator=(other);
	}

	PropertyBase()
	{
		
	}

	virtual T Get() const= 0;
	virtual void Set(const T &value)= 0;

	virtual operator T() const
	{
		return Get();
	}

	virtual PropertyBase & operator=(const T &value)
	{
		Set(value);
		
		return *this;
	}

	virtual PropertyBase & operator=(const PropertyBase &other)
	{
		Set(other.Get());

		return *this;
	}

	virtual bool operator==(const T &other) const
	{
		return Get()== other;
	}

	virtual bool operator==(const PropertyBase<T> &other)
	{
		return Get()== other.Get();
	}

	virtual bool operator!=(const T &other) const
	{
		return Get()!= other;
	}

	virtual bool operator!=(const PropertyBase<T> &other)
	{
		return Get()!= other.Get();
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

public:
	Property(const Property &other)
	{
		operator=(other);
	}

	Property(T *value_pointer)
	{
		this->value_pointer= value_pointer;
	}

	using PropertyBase::operator=;

	Property & operator=(const Property &other)
	{
		Set(other.Get());

		return *this;
	}

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
	Getter & operator=(const Getter &getter);

	void Set(const T &value)
	{
		assert(false && "Getter::Set: Not supposed to call this");
	}

public:
	Getter(const Getter &other)
	{
		operator=(other);
	}

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