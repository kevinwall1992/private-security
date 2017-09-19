#ifndef PS_PROPERTY_SPECIALIZATIONS
#define PS_PROPERTY_SPECIALIZATIONS

#include "Property.h"
#include "Vector.h"


template<class T>
class Subproperty
{
public:
};


template<class U>
class Subproperty<Vector3<U>> : public PropertyBase<U>
{
	PropertyBase<Vector3<U>> *property_;
	int element_index;

public:
	Subproperty(PropertyBase<Vector3<U>> *property_, int element_index)
	{
		this->property_= property_;
		this->element_index= element_index;
	}

	using PropertyBase::operator=;

	virtual U Get() const
	{
		return property_->Get().c[element_index];
	}

	virtual void Set(const U &value)
	{
		Vector3<U> vector= property_->Get();
		vector.c[element_index]= value;

		property_->Set(vector);
	}
};


template<class U>
class Subproperty<Vector2<U>> : public PropertyBase<U>
{
	PropertyBase<Vector2<U>> *property_;
	int element_index;

public:
	Subproperty(PropertyBase<Vector2<U>> *property_, int element_index)
	{
		this->property_= property_;
		this->element_index= element_index;
	}

	using PropertyBase::operator=;

	virtual U Get() const
	{
		return property_->Get().c[element_index];
	}

	virtual void Set(const U &value)
	{
		Vector2<U> vector= property_->Get();
		vector.c[element_index]= value;

		property_->Set(vector);
	}
};


template<class U>
class Property<Vector3<U>> : public PropertyBase<Vector3<U>>
{
	Vector3<U> *value_pointer;

public:
	Subproperty<Vector3<U>> x, y, z;

	Property(const Property &other)
	{
		operator=(other);
	}

	Property(Vector3<U> *value_pointer)
		: x(this, 0), y(this, 1), z(this, 2)
	{
		this->value_pointer= value_pointer;
	}

	using PropertyBase::operator=;

	Property & operator=(const Property &other)
	{
		Set(other.Get());

		return *this;
	}

	virtual Vector3<U> Get() const
	{
		return *value_pointer;
	}

	virtual void Set(const Vector3<U> &value)
	{
		value_pointer->operator=(value);
	}
};

typedef Property<Vec3f> Vec3fProperty;
typedef Property<Vec3i> Vec3iProperty;


template<class U>
class Property<Vector2<U>> : public PropertyBase<Vector2<U>>
{
	Vector2<U> *value_pointer;

public:
	Subproperty<Vector2<U>> x, y;

	Property(const Property &other)
	{
		operator=(other);
	}

	Property(Vector2<U> *value_pointer)
		: x(this, 0), y(this, 1)
	{
		this->value_pointer= value_pointer;
	}

	using PropertyBase::operator=;

	Property & operator=(const Property &other)
	{
		Set(other.Get());

		return *this;
	}

	virtual Vector2<U> Get() const
	{
		return *value_pointer;
	}

	virtual void Set(const Vector2<U> &value)
	{
		value_pointer->operator=(value);
	}
};

typedef Property<Vec2f> Vec2fProperty;
typedef Property<Vec2i> Vec2iProperty;


//Should this use private inheritance?
template<class T>
class GetterSubproperty : public Subproperty<T>
{
	void Set(const T &value)
	{
		assert(false && "GetterSubproperty::Set: Not supposed to call this");
	}

	using Subproperty<T>::operator=;
	using Subproperty<T>::operator+=;
	using Subproperty<T>::operator-=;
	using Subproperty<T>::operator*=;
	using Subproperty<T>::operator/=;

public:
	GetterSubproperty(PropertyBase<T> *property_, int element_index)
		: Subproperty(property_, element_index)
	{
		
	}
};


template<class U>
class Getter<Vector3<U>> : private Property<Vector3<U>>
{
	Getter & operator=(const Getter &getter);

public:
	GetterSubproperty<Vector3<U>> x, y, z;

	Getter(const Getter &other)
	{
		operator=(other);
	}

	Getter(Vector3<U> *value_pointer)
		: Property<Vector3<U>>(value_pointer), x(this, 0), y(this, 1), z(this, 2)
	{
		
	}

	virtual operator Vector3<U>()
	{
		return Get();
	}

	using Property<Vector3<U>>::operator==;
	using Property<Vector3<U>>::operator+;
	using Property<Vector3<U>>::operator-;
	using Property<Vector3<U>>::operator*;
	using Property<Vector3<U>>::operator/;

	using Property<Vector3<U>>::Get;
};

typedef Getter<Vec3f> Vec3fGetter;
typedef Getter<Vec3i> Vec3iGetter;


template<class U>
class Getter<Vector2<U>> : private Property<Vector2<U>>
{
	Getter & operator=(const Getter &getter);

public:
	GetterSubproperty<Vector2<U>> x, y;

	Getter(const Getter &other)
	{
		operator=(other);
	}

	Getter(Vector2<U> *value_pointer)
		: Property<Vector2<U>>(value_pointer), x(this, 0), y(this, 1)
	{
		
	}

	virtual operator Vector2<U>()
	{
		return Get();
	}

	using Property<Vector2<U>>::operator==;
	using Property<Vector2<U>>::operator+;
	using Property<Vector2<U>>::operator-;
	using Property<Vector2<U>>::operator*;
	using Property<Vector2<U>>::operator/;

	using Property<Vector2<U>>::Get;
};

typedef Getter<Vec2f> Vec2fGetter;
typedef Getter<Vec2i> Vec2iGetter;

#endif