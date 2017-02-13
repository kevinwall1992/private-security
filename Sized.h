#ifndef PS_RESIZABLE
#define PS_RESIZABLE

#include "Mutable.h"
#include "Property.h"


class Sized : public Updatable
{
	int width= -1, height= -1;

public:
	ImmutableProperty<int> Width;
	ImmutableProperty<int> Height;

	Sized();

	float GetAspectRatio();

	virtual void Resize(int width, int height);
};

//Need changes to width and height to result in Resize
class Resizable : public Sized
{
protected:
	virtual void Initialize_Sized()= 0;
	virtual void Free_Sized()= 0;

public:
	virtual void Resize(int width, int height);
};

#endif
