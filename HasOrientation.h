#ifndef PS_HAS_ORIENTATION
#define PS_HAS_ORIENTATION

#include "Math.h"

class HasPosition
{
public:
	virtual Vec3f GetPosition()= 0;
};

class HasOrientation : public HasPosition
{
public:
	virtual float GetRotation()= 0;
};

#endif