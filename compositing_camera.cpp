#include "CompositingCamera.h"


void CompositingCamera::Initialize(int width, int height)
{
	if(initialized)
		return;

	diffuse_map= ColorImage(width, height);
	glossiness_map= ColorImage(width, height);
	normal_map= ColorImage(width, height);
	depth_map= DepthImage(width, height);

	initialized= true;
}

void CompositingCamera::ResizeResizables(int width, int height)
{
	diffuse_map.Resize(width, height);
	glossiness_map.Resize(width, height);
	normal_map.Resize(width, height);
	depth_map.Resize(width, height);

	Touch();
}

void CompositingCamera::Update()
{
	RayCameraBase::Update();

	depth_map.Clear(1.0f);
}

//Eventually this should spawn threads
void CompositingCamera::GenerateMaps(Scene &scene, int width, int height)
{
	Initialize(width, height);
	ResizeResizables(width, height);
	CatchUp();

	RayPacketBlock ray_packet_block;

	for(int tile_index= 0; tile_index< (width/ 64)* (height/ 32); tile_index++)
	{
		Vec3f forward= GetForward();
		Vec3f position= Position;

		int tile_count_x= width/ 64;
		int tile_x= tile_index% tile_count_x;
		int tile_y= tile_index/ tile_count_x;
		int x_offset= tile_x* 64;
		int y_offset= tile_y* 32;

		ispc::GetRayPackets_Compositing(x_offset, y_offset, 
										reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
										reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
										width, height,
										reinterpret_cast<ispc::RayPacket *>(ray_packet_block.ray_packets), 
										RAY_PACKET_BLOCK_SIZE);
	
		for(int i= 0; i< RAY_PACKET_BLOCK_SIZE; i++)
			scene.Intersect(ray_packet_block.ray_packets[i]);

		ISPCMesh *meshes= scene.GetISPCMeshes();
		int *material_ids= scene.GetMaterialIDs();

		ispc::Interpolate_Coherent(reinterpret_cast<ispc::RayPacket *>(ray_packet_block.ray_packets), 
									RAY_PACKET_BLOCK_SIZE,
									meshes, material_ids);

		ISPCMaterial *materials= scene.GetISPCMaterials();
		ispc::GenerateCompositingMaps(reinterpret_cast<ispc::RayPacket *>(ray_packet_block.ray_packets),
										RAY_PACKET_BLOCK_SIZE,
										meshes, materials, 
										GetProjectedTransform(width, height).GetMatrix().c,
										reinterpret_cast<uint8_t *>(diffuse_map.GetPixels()),
										reinterpret_cast<uint8_t *>(glossiness_map.GetPixels()),
										reinterpret_cast<uint8_t *>(normal_map.GetPixels()), 
										depth_map.GetPixels(), 
										width);
	}
}

CompositingCamera::CompositingCamera(float fov, Vec3f position)
	: RayCameraBase(fov, position)
{
	
}

ColorImage CompositingCamera::GetDiffuseMap()
{
	return diffuse_map;
}

ColorImage CompositingCamera::GetGlossinessMap()
{
	return glossiness_map;
}

ColorImage CompositingCamera::GetNormalMap()
{
	return normal_map;
}

DepthImage CompositingCamera::GetDepthMap()
{
	return depth_map;
}

Photo CompositingCamera::TakePhoto(Scene &scene, int width, int height)
{
	GenerateMaps(scene, width, height);

	return Photo(diffuse_map);
}
