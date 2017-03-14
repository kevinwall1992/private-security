#ifndef PS_HAS_POSITION
#define PS_HAS_POSITION

#include "Math.h"


class HasPosition
{
public:
	virtual Vec3f GetPosition()= 0;
};

#endif