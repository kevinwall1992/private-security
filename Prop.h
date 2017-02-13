#ifndef EBR_PROP
#define EBR_PROP

#include "Mesh.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsSystem.h"
#include "Raytraceable.h"
#include "Rasterizable.h"


class Prop : public Raytraceable, public Rasterizable
{
public:
	struct DrawFlags { enum Enum { None= 0, RasterizeGbuffers= 1 }; };

protected:
	DrawFlags::Enum draw_flags= DrawFlags::None;

public:
	Prop();

	void AddDrawFlags(DrawFlags::Enum draw_flags);
	void RemoveDrawFlags(DrawFlags::Enum draw_flags);
	bool AreDrawFlagsActive(DrawFlags::Enum draw_flags);//This is an "any", not "all"

	virtual void RasterizeConditionally(DrawFlags::Enum draw_flags);//Same
};

class PropContainer : public Prop
{
public:
	virtual vector<Prop *> GetProps()= 0;

public:
	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
	virtual void RasterizeConditionally(DrawFlags::Enum draw_flags);
};

class BasicPropContainer : public PropContainer
{
	vector<Prop *> props;

public:
	vector<Prop *> GetProps();

public:
	void AddProp(Prop *prop);
};

class MeshProp : public Prop
{
	Mesh *mesh;
	Vec3f displacement= Vec3f(0.0f, 0.0f, 0.0f);
	float rotation= 0.0f;

	GLuint vao_handle;
	GLuint element_buffer_handle;

	bool is_initialized= false;
	void Initialize();

public:
	MeshProp(Mesh *mesh);

	void SetDisplacement(Vec3f displacement);
	void SetRotation(float rotation);

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
};


#endif