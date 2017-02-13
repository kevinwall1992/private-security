#ifndef PS_RAYTRACEABLE
#define RAYTRACEABLE

#include "EmbreeSystem.h"
#include "Common.h"


class RaytracingPrimitive
{
	bool has_direct_visibility;
	int geometry_id;

protected:
	void SetGeometryID(int geometry_id);

public:
	RaytracingPrimitive(bool has_direct_visibility);

	bool HasDirectVisibility();
	int GetGeometryID();

	virtual void AddToEmbreeScene(RTCScene &scene)= 0;
};


struct Mesh;

//Should this inherit from Mesh?
class RaytracingMesh : public RaytracingPrimitive
{
	Mesh *mesh;

public:
	RaytracingMesh(bool has_direct_visibility, Mesh *mesh);

	Mesh * GetMesh();

	void AddToEmbreeScene(RTCScene &embree_scene);

	ISPCMesh MakeISPCMesh();
};


class Raytraceable
{
public:
	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives()= 0;
};

#endif