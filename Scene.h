#ifndef EBR_SCENE
#define EBR_SCENE

#include "Common.h"
#include "EmbreeSystem.h"
#include "Light.h"
#include "Prop.h"
#include "ISPCKernels.h"

struct ISPCLighting : public ispc::Lighting { ISPCLighting(); ~ISPCLighting(); };
typedef ispc::Mesh ISPCMesh;
typedef ispc::PhongMaterial ISPCMaterial;

class Scene
{
	RTCScene embree_scene;
	vector<unsigned int> geometry_ids;
	vector<Light *> lights;
	vector<AmbientLight *> ambient_lights;

	ISPCLighting ispc_lighting;
	vector<ISPCMesh> ispc_meshes;
	vector<ISPCMaterial> ispc_materials;
	vector<int> material_ids;

	bool commited= false;

	vector<Prop> props;

	void BuildISPCLighting();
	void BuildISPCMeshes();
	void BuildISPCMaterials();
	void BuildISPCData();

public:
	Scene();
	~Scene();

	void AddProp(Prop prop);
	void AddProps(vector<Prop> props);
	Prop * GetProp(int geometry_id);

	void AddLight(Light *light);
	vector<Light *> * GetLights();
	vector<AmbientLight *> * GetAmbientLights();

	ISPCLighting * GetISPCLighting();
	ISPCMesh * GetISPCMeshes();
	ISPCMaterial * GetISPCMaterials();
	int * GetMaterialIDs();
	RTCScene * GetEmbreeScene();

	void Commit();

	void Intersect(Ray &ray);
	void Intersect(Ray *rays, int count, bool is_coherent);
	void Intersect(RayPacket &ray_packet);
	void Intersect(RayPacket *ray_packets, int count, bool is_coherent);

	void Intersect_Occlusion(Ray *rays, int count, bool is_coherent);
	void Intersect_Occlusion(RayPacket &ray_packet);

	void Interpolate(RayPacket &ray_packet, RayPacketExtras &ray_packet_extras);
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