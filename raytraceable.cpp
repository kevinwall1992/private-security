#include "Raytraceable.h"
#include "Mesh.h"

void RaytracingPrimitive::SetGeometryID(int geometry_id_)
{
	geometry_id= geometry_id_;
}

RaytracingPrimitive::RaytracingPrimitive(bool has_direct_visibility_)
{
	has_direct_visibility= has_direct_visibility_;
}

bool RaytracingPrimitive::HasDirectVisibility()
{
	return has_direct_visibility;
}

int RaytracingPrimitive::GetGeometryID()
{
	return geometry_id;
}

RaytracingMesh::RaytracingMesh(bool has_direct_visibility, Mesh *mesh_, Transform transform_)
	: RaytracingPrimitive(has_direct_visibility)
{
	mesh= mesh_;
	transform= transform_;
}

Mesh * RaytracingMesh::GetMesh()
{
	return mesh;
}

void RaytracingMesh::AddToEmbreeScene(RTCScene &embree_scene)
{
	int vertex_count= mesh->GetVertexCount();
	int triangle_count= mesh->GetTriangleCount();
	unsigned int geometry_id= rtcNewTriangleMesh(embree_scene, RTC_GEOMETRY_STATIC, triangle_count, vertex_count);
	EmbreeVertex *vertices= (EmbreeVertex *)rtcMapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	for(int j= 0; j< vertex_count; j++)
	{
		Vec3f transformed_vertex= transform.Apply(*reinterpret_cast<Vec3f *>((&mesh->positions[j* 3+ 0])));

		vertices[j].x= transformed_vertex.x;
		vertices[j].y= transformed_vertex.y;
		vertices[j].z= transformed_vertex.z;
		vertices[j].w= 0;
	}
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	EmbreeTriangle *triangles= (EmbreeTriangle *)rtcMapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);
	memcpy(triangles, &mesh->position_indices[0], sizeof(int)* triangle_count* 3);
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);

#if ISPC_INTERPOLATION == 0
	rtcSetBuffer(embree_scene, geometry_id, RTC_USER_VERTEX_BUFFER0, &(mesh->normals[0]), 0, sizeof(Vec3f));
#endif

	cout << mesh->GetName() << " triangles: " << mesh->GetTriangleCount() << endl;

	SetGeometryID(geometry_id);
}

ISPCMesh RaytracingMesh::MakeISPCMesh()
{
	ISPCMesh ispc_mesh;

	ispc_mesh.positions= &(mesh->positions[0]);
	ispc_mesh.position_indices= &(mesh->position_indices[0]);

	ispc_mesh.normals= &(mesh->normals[0]);
	ispc_mesh.normal_indices= &(mesh->normal_indices[0]);
		
	if(mesh->texture_coordinates.size()> 0)
	{
		ispc_mesh.texture_coordinates= &(mesh->texture_coordinates[0]);
		ispc_mesh.texture_coordinate_indices= &(mesh->texture_coordinate_indices[0]);
	}
	else
	{
		ispc_mesh.texture_coordinates= nullptr;
		ispc_mesh.texture_coordinate_indices= nullptr;
	}

	ispc_mesh.indirect_only= !HasDirectVisibility();

	return ispc_mesh;
}
