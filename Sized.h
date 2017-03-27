#ifndef PS_RESIZABLE
#define PS_RESIZABLE

#include "Property.h"
#include "Vector.h"


class Sized
{
protected:
	Vec2i size= Vec2i(-1, -1);

public:
	Vec2iGetter Size;
	IntGetter Width, Height;

	Sized(Vec2i size);
	Sized();

	float GetAspectRatio();
};


class Resizable;

template<class T>
class ResizeProperty
{
public:
	
};

template<>
class ResizeProperty<Vec2i> : public Property<Vec2i>
{
	Resizable *resizable;

public:
	ResizeProperty(Vec2i *value_pointer, Resizable *resizable)
		: Property<Vec2i>(value_pointer)
	{
		this->resizable= resizable;
	}

	using PropertyBase<Vec2i>::operator=;

	virtual void Set(const Vec2i &value);
};

template<>
class ResizeProperty<int> : public PropertyBase<int>
{
	Resizable *resizable;
	int element_index;

public:
	ResizeProperty(Resizable *resizable, int element_index)
	{
		this->resizable= resizable;
		this->element_index= element_index;
	}

	using PropertyBase<int>::operator=;

	virtual int Get() const;
	virtual void Set(const int &value);
};

class Resizable : public Sized
{
	using Sized::size;

protected:
	void SetSize(Vec2i size);

public:
	ResizeProperty<Vec2i> Size;
	ResizeProperty<int> Width, Height;

	Resizable(Vec2i size);
	Resizable();

	virtual void Resize(Vec2i size);
};

inline void ResizeProperty<Vec2i>::Set(const Vec2i & value)
{
	resizable->Resize(value);
}

inline int ResizeProperty<int>::Get() const
{
	return resizable->Size.Get().c[element_index];
}

inline void ResizeProperty<int>::Set(const int & value)
{
	Vec2i size= resizable->Size;
	size.c[element_index]= value;

	resizable->Resize(size);
}

#endif
