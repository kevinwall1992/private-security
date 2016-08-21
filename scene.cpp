#include "Scene.h"
#include "Mesh.h"

#if STREAM_MODE_
#define RTC_INTERSECT_MODE RTC_INTERSECT_STREAM
#elif PACKET_MODE_
#define RTC_INTERSECT_MODE JOIN(RTC_INTERSECT, PACKET_SIZE)
#else
#define RTC_INTERSECT_MODE RTC_INTERSECT1
#endif

Scene::Scene()
{
	//need to determine if we need a second stream for packeted intersection
	//interesting to know (as well) whether the stream mode doesn't like switching between packets and singles
	embree_scene = rtcDeviceNewScene(System::embree.device, RTC_SCENE_STATIC, RTC_INTERSECT_MODE);
}

//Little bit scary... but it really should be Scene's responsibility to clean this up...
//Guee we may have to use dynamic instances if this becomes a problem

//Need to delete lights
Scene::~Scene()
{
	rtcDeleteScene(embree_scene);
}

void Scene::AddOBJ(string filename)
{
	Mesh raw_mesh(filename);
	int vertex_count= raw_mesh.GetVertexCount();
	int triangle_count= raw_mesh.GetTriangleCount();
	unsigned int geometry_id= rtcNewTriangleMesh(embree_scene, RTC_GEOMETRY_STATIC, triangle_count, vertex_count);
	Vertex *vertices= (Vertex *)rtcMapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	for(unsigned int i= 0; i< vertex_count; i++)
	{
		vertices[i].x= raw_mesh.positions[i* 3+ 0];
		vertices[i].y= raw_mesh.positions[i* 3+ 1];
		vertices[i].z= raw_mesh.positions[i* 3+ 2];
		vertices[i].w= 0;
	}
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	Triangle *triangles= (Triangle *)rtcMapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);
	memcpy(triangles, &raw_mesh.position_indices[0], sizeof(int)* triangle_count* 3);
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);

	cout << "triangles: " << raw_mesh.GetTriangleCount() << endl;

	geometry_ids.push_back(geometry_id);
}

void Scene::AddLight(Light *light)
{
	if(light->IsAmbient())
		ambient_lights.push_back(dynamic_cast<AmbientLight *>(light));
	else
		lights.push_back(light);
}

vector<Light *> * Scene::GetLights()
{
	return &lights;
}

vector<AmbientLight *> * Scene::GetAmbientLights()
{
	return &ambient_lights;
}

ispc::Lighting Scene::GetISPCLighting()
{
	ispc::Lighting lighting;

	lighting.point_lights= new ispc::PointLight_[lights.size()];
	for(unsigned int i= 0; i< lights.size(); i++)
	{
		Vec3f position= lights[i]->GetPosition();
		SetFloat3(lighting.point_lights[i].position, position);

		Color intensity= lights[i]->GetIntensity();
		SetFloat3(lighting.point_lights[i].intensity, intensity);
	}
	lighting.point_light_count= lights.size();

	Vec3f ambient;
	for(unsigned int i= 0; i< ambient_lights.size(); i++)
		ambient+= ambient_lights[i]->GetIntensity();
	SetFloat3(lighting.ambient, ambient);

	return lighting;
}

void Scene::Commit()
{
	rtcCommit(embree_scene);
	commited= true;
}

void Scene::Intersect(Ray &ray)
{
	rtcIntersect(embree_scene, ray);
}

void Scene::Intersect(Ray *rays, int count, bool is_coherent)
{
	RTCIntersectContext context;
	context.flags= is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;

	rtcIntersect1M(embree_scene, &context, rays, count, sizeof(Ray));
}

#define rtcIntersectPacket JOIN(rtcIntersect, PACKET_SIZE)
void Scene::Intersect(RayPacket &ray_packet)
{
#if STREAM_MODE_
	assert(false && "Attempted to intersect single packet in stream mode.");

#else
	int32_t valid[PACKET_SIZE];
	memset(valid, 0xFFFFFFFF, sizeof(int32_t)* PACKET_SIZE);
	
	rtcIntersectPacket(valid, embree_scene, ray_packet);

#endif
}

void Scene::Intersect(RayPacket *ray_packet, int count, bool is_coherent)
{
#if STREAM_MODE_
	RTCIntersectContext context;
	context.flags= is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;

	rtcIntersectNM(embree_scene, &context, ray_packet, PACKET_SIZE, count, sizeof(RayPacket));

#else
	assert(false && "Attempted to intersect packet stream in single mode.");

#endif
}