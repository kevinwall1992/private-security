/*#ifndef PS_VISIBLE
#define PS_VISIBLE

#include "Mesh.h"
#include "Math.h"
#include "Texture.h"
#include "GraphicsSystem.h"


class Visible
{
public:
	virtual void Draw()= 0;
};

class MeshVisible
{
	Mesh *mesh;
	Vec3f displacement= Vec3f(0.0f, 0.0f, 0.0f);
	float rotation= 0.0f;

	GLuint vertex_array_handle;
	GLuint element_buffer_handle;

public:
	MeshVisible(Mesh *mesh);

	void SetDisplacement(Vec3f displacement);
	void SetRotation(float rotation);

	virtual void Draw();
};

class BakedVisible
{
	Texture *diffuse_texture;
	DepthTexture *depth_texture;
	Texture *normal_map;

public:
	BakedVisible(Texture *diffuse_texture, DepthTexture *depth_texture, Texture *normal_map);

	void SetDisplacement(Vec3f displacement);
	void SetRotation(float rotation);

	virtual void Draw();
};

#endif*/