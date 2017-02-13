#ifndef EBR_SCENE
#define EBR_SCENE

#include "Common.h"
#include "EmbreeSystem.h"
#include "Light.h"
#include "Prop.h"
#include "Ray.h"


class Scene : public BasicPropContainer
{
	RTCScene embree_scene;

	vector<RaytracingPrimitive *> primitives;
	vector<Light *> lights;
	vector<AmbientLight *> ambient_lights;

	ISPCLighting ispc_lighting;
	vector<ISPCMesh> ispc_meshes;
	vector<ISPCMaterial> ispc_materials;
	vector<int> material_ids;

	bool commited= false;


	void AddPrimitive(RaytracingPrimitive *primitive);

	void BuildISPCLighting();
	void BuildISPCMeshes();
	void BuildISPCMaterials();
	void BuildISPCData();

public:
	Scene();
	~Scene();

	RaytracingPrimitive * GetPrimitiveByGeometryID(int geometry_id);

	void AddLight(Light *light);
	vector<Light *> GetLights();
	vector<AmbientLight *> GetAmbientLights();

	ISPCLighting * GetISPCLighting();
	ISPCMesh * GetISPCMeshes();
	ISPCMaterial * GetISPCMaterials();
	int * GetMaterialIDs();
	RTCScene * GetEmbreeScene();

	void Commit();

	void Intersect(Ray &ray);
	void Intersect(Ray *rays, int count, bool is_coherenty);
	void Intersect(RayPacket &ray_packety);
	void Intersect(RayPacket *ray_packets, int count, bool is_coherent);

	void Intersect_Visibility(VisibilityRay *rays, int count, bool is_coherent);
	void Intersect_Visibility(VisibilityRayPacket &ray_packet);

	void Interpolate(RayPacket &ray_packet);
};

#endif