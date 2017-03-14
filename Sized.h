#ifndef PS_RESIZABLE
#define PS_RESIZABLE

#include "Mutable.h"
#include "Property.h"
#include "Vector.h"


//Think Sized may need to go, or somehow be changed to be more generic***
//It even has a Resize method, despite Resizable's existance...
//Problem is that Properties are really nice, but I can't restrict 
//or enable mutation of Width and Height, either of which is a requirement for
//some subclasses and not others. 
//Complete redesign of these classes needed. 
class Sized : public Updatable
{
	Vec2i size= Vec2i(-1, -1);

public:
	ImmutableProperty<int> Width;
	ImmutableProperty<int> Height;

	Sized();

	Vec2i GetSize() const;
	float GetAspectRatio();

	virtual void Resize(Vec2i size);
};

//Need changes to width and height to result in Resize***
class Resizable : public Sized
{
protected:
	virtual void Initialize_Sized()= 0;
	virtual void Free_Sized()= 0;

public:
	virtual void Resize(Vec2i size);
};

#endif
