#ifndef PS_PHYSICAL
#define PS_PHYSICAL

#include "Vector.h"
#include "Chronon.h"
#include "HasPosition.h"

struct Physical : public virtual Chronal, public virtual HasPosition
{
	Vec3f position;
	Vec3f velocity;

	Vec3f GetPosition();

	void ApplyForce(Vec3f force, Chronons duration);

	void Step(Chronons chronons);
};

#endif