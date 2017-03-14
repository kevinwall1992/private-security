#include "Scene.h"
#include "Mesh.h"
#include "Timer.h"

ISPCLighting::ISPCLighting()
{
	point_lights= nullptr;
}

ISPCLighting::~ISPCLighting()
{
	if(point_lights!= nullptr)
		delete point_lights;
}


void Scene::AddPrimitive(RaytracingPrimitive *primitive)
{
	primitive->AddToEmbreeScene(embree_scene);
	primitives.push_back(primitive);
}

void Scene::BuildISPCLighting()
{
	ispc_lighting= ISPCLighting();
	ispc_lighting.point_lights= new ispc::PointLight[lights.size()];
	for(unsigned int i= 0; i< lights.size(); i++)
	{
		Vec3f position= lights[i]->GetPosition();
		Math::SetFloat3(ispc_lighting.point_lights[i].position, position);

		Color intensity= lights[i]->GetIntensity();
		Math::SetFloat3(ispc_lighting.point_lights[i].intensity, intensity);
	}
	ispc_lighting.point_light_count= (int)lights.size();

	Vec3f ambient(0.0f, 0.0f, 0.0f);
	for(unsigned int i= 0; i< ambient_lights.size(); i++)
		ambient+= ambient_lights[i]->GetIntensity();
	Math::SetFloat3(ispc_lighting.ambient, ambient);
}

void Scene::BuildISPCMeshes()
{
	for(unsigned int i= 0; i< primitives.size(); i++)
		ispc_meshes.push_back(dynamic_cast<RaytracingMesh *>(primitives[i])->MakeISPCMesh());
}

void Scene::BuildISPCMaterials()
{
	vector<Material *> materials= Material::GetMaterials();
	for(unsigned int i= 0; i< materials.size(); i++)
	{
		ISPCMaterial ispc_material;
		PhongMaterial *material= dynamic_cast<PhongMaterial *>(materials[i]);

		Math::SetFloat3(ispc_material.diffuse, material->diffuse);
		Math::SetFloat3(ispc_material.specular, material->specular);
		ispc_material.glossiness= material->glossiness;
		ispc_material.reflectivity= material->reflectivity;
		ispc_material.refractive_index= material->refractive_index;
		ispc_material.transparency= material->transparency;

		ispc_materials.push_back(ispc_material);
	}

	for(unsigned int i= 0; i< primitives.size(); i++)
	{
		if(primitives[i]->GetGeometryID()>= material_ids.size())
			material_ids.resize(primitives[i]->GetGeometryID()+ 1);

		Material *material= dynamic_cast<RaytracingMesh *>(primitives[i])->GetMesh()->material;
		int material_id;
		for(unsigned int j= 0; j< materials.size(); j++)
			if(materials[j]== material)
			{
				material_id= j;
				break;
			}

		material_ids[primitives[i]->GetGeometryID()]= material_id;
	}
}

void Scene::BuildISPCData()
{
	BuildISPCLighting();
	BuildISPCMeshes();
	BuildISPCMaterials();
}

#if STREAM_MODE
#define RTC_INTERSECT_MODE 0
#elif defined PACKET_MODE
#define RTC_INTERSECT_MODE JOIN(RTC_INTERSECT, PACKET_SIZE)
#else
#define RTC_INTERSECT_MODE RTC_INTERSECT1
#endif

Scene::Scene()
{
	//need to determine if we need a second stream for packeted intersection
	//interesting to know (as well) whether the stream mode doesn't like switching between packets and singles

	embree_scene = rtcDeviceNewScene(System::embree.device, RTC_SCENE_STATIC, (RTCAlgorithmFlags)(RTC_INTERSECT_MODE | (ISPC_INTERPOLATION ? 0 : RTC_INTERPOLATE) | RTC_INTERSECT_STREAM));
}

Scene::~Scene()
{
	for(unsigned int i= 0; i< lights.size(); i++)
		delete lights[i];

	rtcDeleteScene(embree_scene);
}

/*void Scene::AddMesh(Mesh *mesh)
{
	int vertex_count= mesh->GetVertexCount();
	int triangle_count= mesh->GetTriangleCount();
	unsigned int geometry_id= rtcNewTriangleMesh(embree_scene, RTC_GEOMETRY_STATIC, triangle_count, vertex_count);
	Vertex *vertices= (Vertex *)rtcMapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	for(int j= 0; j< vertex_count; j++)
	{
		vertices[j].x= mesh->positions[j* 3+ 0];
		vertices[j].y= mesh->positions[j* 3+ 1];
		vertices[j].z= mesh->positions[j* 3+ 2];
		vertices[j].w= 0;
	}
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	Triangle *triangles= (Triangle *)rtcMapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);
	memcpy(triangles, &mesh->position_indices[0], sizeof(int)* triangle_count* 3);
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);

#if ISPC_INTERPOLATION == 0
	rtcSetBuffer(embree_scene, geometry_id, RTC_USER_VERTEX_BUFFER0, &(mesh->normals[0]), 0, sizeof(Vec3f));
#endif

	cout << mesh->GetName() << " triangles: " << mesh->GetTriangleCount() << endl;

	geometry_ids.push_back(geometry_id);

	meshes.push_back(mesh);
}*/

/*void Scene::AddProps(vector<Prop> props)
{
	for(unsigned int i= 0; i< props.size(); i++)
		AddProp(props[i]);
}*/

RaytracingPrimitive * Scene::GetPrimitiveByGeometryID(int geometry_id)
{
	for(unsigned int i= 0; i< primitives.size(); i++)
	{
		if(geometry_id== primitives[i]->GetGeometryID())
			return primitives[i];
	}

	assert(false && "Called Scene::GetProp with invalid geometry_id.");
	return nullptr;
}

void Scene::AddLight(Light *light)
{
	if(light->IsAmbient())
		ambient_lights.push_back(dynamic_cast<AmbientLight *>(light));
	else
		lights.push_back(light);
}

vector<Light *> Scene::GetLights()
{
	return lights;
}

vector<AmbientLight *> Scene::GetAmbientLights()
{
	return ambient_lights;
}

ISPCLighting * Scene::GetISPCLighting()
{
	return &ispc_lighting;	
}

//This isn't quite right, we can't assume that geometry ids map 1:1 as well as 
//are exactly equal to mesh ids. Should have mesh id array that maps geometry 
//ids to mesh ids. 
ISPCMesh * Scene::GetISPCMeshes()
{
	return &(ispc_meshes[0]);
}

ISPCMaterial * Scene::GetISPCMaterials()
{
	return &(ispc_materials[0]);
}

RTCScene * Scene::GetEmbreeScene()
{
	return &embree_scene;
}

//Assumes that geometry ids are not arbitrary- if theres a geometry id 
//1000000, then we will end up having a very long array.
int * Scene::GetMaterialIDs()
{
	return &(material_ids[0]);
}

void Scene::Commit()
{
	vector<RaytracingPrimitive *> meshes= GetRaytracingPrimitives();
	for(unsigned int i= 0; i< meshes.size(); i++)
		AddPrimitive(meshes[i]);
	rtcCommit(embree_scene);

	BuildISPCData();

	commited= true;
}

void Scene::Intersect(EBRRay &ray)
{
	rtcIntersect(embree_scene, ray);
}

void Scene::Intersect(EBRRay *rays, int count, bool is_coherent)
{
	RTCIntersectContext context;
	context.flags= is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;

	rtcIntersect1M(embree_scene, &context, rays, count, sizeof(EBRRay));
}

#define rtcIntersectPacket JOIN(rtcIntersect, PACKET_SIZE)
void Scene::Intersect(EBRRayPacket &ray_packet)
{
#if STREAM_MODE
	assert(false && "Attempted to intersect single packet in stream mode.");

#else
	int32_t activity_mask[PACKET_SIZE];
	memset(activity_mask, 0xFFFFFFFF, sizeof(int32_t)* PACKET_SIZE);
	rtcIntersectPacket(activity_mask, embree_scene, ray_packet);

#endif
}

void Scene::Intersect(EBRRayPacket *ray_packets, int count, bool is_coherent)
{
	RTCIntersectContext context;
	context.flags= is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;

	rtcIntersectNM(embree_scene, &context, reinterpret_cast<RTCRayN *>(ray_packets), PACKET_SIZE, count, sizeof(EBRRayPacket));
}

void Scene::Intersect_Visibility(VisibilityRay *rays, int count, bool is_coherent)
{
#if STREAM_MODE || !PACKETED_SECONDARY_RAYS
	RTCIntersectContext context;
	context.flags= is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;

	rtcOccluded1M(embree_scene, &context, rays, count, sizeof(VisibilityRay));
#else
	assert(false && "Attempted to intersect packet stream in single mode.");
#endif
}

#define rtcOccludedPacket JOIN(rtcOccluded, PACKET_SIZE)
void Scene::Intersect_Visibility(VisibilityRayPacket &ray_packet)
{
#if STREAM_MODE
	assert(false && "Attempted to occlude single packet in stream mode.");
#else
	int32_t activity_mask[PACKET_SIZE];
	memset(activity_mask, 0xFFFFFFFF, sizeof(int32_t)* PACKET_SIZE);
	rtcOccludedPacket(activity_mask, embree_scene, ray_packet);

#endif
}

void Scene::Interpolate(EBRRayPacket &ray_packet)
{
	int32_t activity_mask[PACKET_SIZE];
	bool active= false;
	for(int j= 0; j< PACKET_SIZE; j++)
		if(ray_packet.geomID[j]!= RTC_INVALID_GEOMETRY_ID)
		{
			activity_mask[j]= 0xFFFFFFFF;
			active= true;
		}
		else
			activity_mask[j]= 0x00000000;
		
	if(active)
	{
		rtcInterpolateN2(embree_scene, 
						 0, 
						 activity_mask, 
						 ray_packet.primID, 
						 ray_packet.u, 
						 ray_packet.v, 
						 PACKET_SIZE, 
						 RTC_USER_VERTEX_BUFFER0, 
						 ray_packet.surface.normal_x, 
						 nullptr, nullptr, nullptr, nullptr, nullptr, 
						 3);
	}
}