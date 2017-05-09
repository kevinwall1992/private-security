#ifndef EBR_PROP
#define EBR_PROP

#include "Mesh.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsSystem.h"
#include "Raytraceable.h"
#include "Rasterizable.h"
#include "VAO.h"


class Prop : public Raytraceable, public Rasterizable
{
public:
	enum DrawFlags { None= 0, Direct= 1, Indirect= 2 };

protected:
	DrawFlags draw_flags= DrawFlags::None;

public:
	Prop();

	void AddDrawFlags(DrawFlags draw_flags);
	void RemoveDrawFlags(DrawFlags draw_flags);
	void SetDrawFlags(DrawFlags draw_flags);
	bool AreDrawFlagsActive_Any(DrawFlags draw_flags);
	bool AreDrawFlagsActive_All(DrawFlags draw_flags);

	virtual void RasterizeConditionally_Any(DrawFlags draw_flags);
	virtual void RasterizeConditionally_All(DrawFlags draw_flags);
};

class PropContainer : public Prop
{
public:
	virtual vector<Prop *> GetProps()= 0;

public:
	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
	virtual void RasterizeConditionally_Any(DrawFlags draw_flags);
	virtual void RasterizeConditionally_All(DrawFlags draw_flags);
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

	VAO vao;
	GLuint element_buffer_handle;

	bool is_initialized= false;
	void Initialize();

	Transform GetModelTransform();

public:
	MeshProp(Mesh *mesh);

	void SetDisplacement(Vec3f displacement);
	void SetRotation(float rotation);

	Mesh * GetMesh();
	VAO GetVAO();

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
};


#endif