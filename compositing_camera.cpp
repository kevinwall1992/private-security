#include "CompositingCamera.h"


void CompositingCamera::Initialize(Vec2i size)
{
	if(initialized)
		return;

	diffuse_color_map= ColorImage(size);
	glossiness_map= ColorImage(size);
	normal_map= ColorImage(size);
	depth_map= DepthImage(size);

	initialized= true;
}

void CompositingCamera::ResizeResizables(Vec2i size)
{
	diffuse_color_map.Resize(size);
	glossiness_map.Resize(size);
	normal_map.Resize(size);
	depth_map.Resize(size);

	Touch();
}

void CompositingCamera::Update()
{
	RayCameraBase::Update();

	depth_map.Clear(1.0f);
}

void CompositingCamera::GenerateMaps(Scene &scene, Vec2i size)
{
	Initialize(size);
	ResizeResizables(size);
	CatchUp();

	RayPacketBlock ray_packet_block;

	for(int tile_index= 0; tile_index< (size.x/ 64)* (size.y/ 32); tile_index++)
	{
		Vec3f forward= GetForward();
		Vec3f position= Position;

		int tile_count_x= size.x/ 64;
		int tile_x= tile_index% tile_count_x;
		int tile_y= tile_index/ tile_count_x;
		int x_offset= tile_x* 64;
		int y_offset= tile_y* 32;

		ispc::GetRayPackets_Compositing(x_offset, y_offset, 
										reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
										reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
										size.x, size.y,
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
										GetProjectedTransform(Math::GetAspectRatio(size)).GetMatrix().c,
										reinterpret_cast<uint8_t *>(diffuse_color_map.GetPixels()),
										reinterpret_cast<uint8_t *>(glossiness_map.GetPixels()),
										reinterpret_cast<uint8_t *>(normal_map.GetPixels()), 
										depth_map.GetPixels(), 
										size.x);
	}
}

CompositingCamera::CompositingCamera(float fov, Vec3f position)
	: RayCameraBase(fov, position)
{
	
}

ColorImage CompositingCamera::GetDiffuseMap()
{
	return diffuse_color_map;
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

PhotoBook CompositingCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	GenerateMaps(scene, size);
	

	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::DiffuseColor, Photo::Type::SpecularColor, Photo::Type::Depth, Photo::Type::Normal };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::DiffuseColor: photo_book[type]= GetDiffuseMap(); break;
			case Photo::Type::SpecularColor: photo_book[type]= GetGlossinessMap(); break;
			case Photo::Type::Depth: photo_book[type]= GetDepthMap(); break;
			case Photo::Type::Normal: photo_book[type]= GetNormalMap(); break;

			case Photo::Type::FullColor:
			default: break;
			}

	return photo_book;
}
