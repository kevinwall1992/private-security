#include "Scene.h"
#include "Mesh.h"


Scene::Scene()
{
	//need to determine if we need a second stream for packeted intersection
	//interesting to know (as well) whether the stream mode doesn't like switching between packets and singles
	embree_scene = rtcDeviceNewScene(System::embree.device, RTC_SCENE_STATIC, RTC_INTERSECT1);
}

//Little bit scary... but it really should be Scene's responsibility to clean this up...
//Guee we may have to use dynamic instances if this becomes a problem
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

void Scene::Commit()
{
	rtcCommit(embree_scene);
	commited= true;
}

void Scene::Intersect(Ray &ray)
{
	rtcIntersect(embree_scene, ray);
}