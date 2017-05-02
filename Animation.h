#ifndef PS_ANIMATION
#define PS_ANIMATION

#include "Prop.h"
#include "Cycle.h"


class Animation : public Prop
{
	MeshProp mesh_prop;
	string action_name;
	float cycles;

	float moment= 0.0f;

	bool initialized= false;

public:
	Animation(Mesh *mesh, string action_name, float cycles= 1);

	void SetDisplacement(Vec3f displacement);
	void SetRotation(float rotation);
	void SetMoment(float moment);

	Mesh * GetMesh();

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
};

#endif
