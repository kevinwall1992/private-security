#ifndef PS_PHYSICAL
#define PS_PHYSICAL

#include "Vector.h"
#include "Chronon.h"
#include "HasOrientation.h"

struct Physical : public virtual Chronal, public virtual HasOrientation
{
	Vec3f position;
	Vec3f velocity;
	float rotation;

	Vec3f GetPosition();
	float GetRotation();

	void ApplyForce(Vec3f force, Chronons duration);

	void Step(Chronons chronons);
};

#endif