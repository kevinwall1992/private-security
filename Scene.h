#ifndef EBR_SCENE
#define EBR_SCENE

#include "Common.h"
#include "EmbreeSystem.h"
#include "Light.h"
#include "ISPCKernels.h"

class Scene
{
	RTCScene embree_scene;
	vector<unsigned int> geometry_ids;
	vector<Light *> lights;
	vector<AmbientLight *> ambient_lights;

	bool commited= false;

public:
	Scene();
	~Scene();

	void AddOBJ(string filename);
	void AddLight(Light *light);

	vector<Light *> * GetLights();
	vector<AmbientLight *> * GetAmbientLights();
	ispc::Lighting GetISPCLighting();

	void Commit();

	void Intersect(Ray &ray);
	void Intersect(Ray *rays, int count, bool is_coherent);
	void Intersect(RayPacket &ray_packet);
	void Intersect(RayPacket *ray_packet, int count, bool is_coherent);
};

struct Vertex
{
	float x, y, z, w;

	Vertex(float x_, float y_, float z_)
	{
		x= x_; y= y_; z= z_;
	}

	Vertex()
	{
		x= 0.0f; y= 0.0f; z= 0.0f;
	}
};

struct Triangle
{
	int a, b, c;

	Triangle(int a_, int b_, int c_)
	{
		a= a_; b= b_; c= c_;
	}

	Triangle()
	{
		a= -1; b= -1; c= -1;
	}
};

#endif