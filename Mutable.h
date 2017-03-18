#ifndef PS_UPDATABLE
#define PS_UPDATABLE

#include "Property.h"


class Mutable
{
	bool was_modified= true;

public:
	virtual bool WasModified();

	virtual void Touch();
	virtual void Scrub();
};


class Sensor : public Mutable
{
public:
	typedef void (*Callback)();

private:
	Mutable *parent;
	Callback callback;

	using Mutable::Scrub;

protected:
	void Touch();

	using Mutable::WasModified;

public:
	Sensor(Mutable *parent, Callback callback= nullptr);
};

template<class T>
class SensorProperty : public Property<T>, public Sensor
{
public:
	SensorProperty(T *value_pointer, Mutable *mutable_)
		: Property<T>(value_pointer), Sensor(mutable_)
	{
		
	}

	using PropertyBase<T>::operator T;
	using Property<T>::operator=;

	virtual void Set(const T &value)
	{
		Property<T>::Set(value);
		Touch();
	}
};

typedef SensorProperty<int> IntSensorProperty;
typedef SensorProperty<float> FloatSensorProperty;
typedef SensorProperty<bool> BoolSensorProperty;

#include "Vector.h"
typedef SensorProperty<Vec3f> Vec3fSensorProperty;
typedef SensorProperty<Vec3i> Vec3iSensorProperty;
typedef SensorProperty<Vec2f> Vec2fSensorProperty;
typedef SensorProperty<Vec2i> Vec2iSensorProperty;


class Checkable : public Mutable
{
	using Mutable::Scrub;
	
protected:
	using Mutable::Touch;

public:
	bool WasModified();
};


class Updatable : public Checkable
{
	using Checkable::WasModified;

protected:
	virtual void Update();
	void CatchUp();

public:

};

#endif