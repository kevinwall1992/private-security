#include "ISPCInterop.h"
#include "ISPCKernels.h"
#include "Camera.h"

void TakeRayPacketBuffer(intptr_t *ray_packet_buffer_id, int ** packet_count, intptr_t camera_id, void **ray_packets, void **ray_packet_extras)
{
	Shutter *shutter= reinterpret_cast<Camera *>(camera_id)->GetShutter();

	RayPacketBlock *ray_packet_block;
	if(*ray_packet_buffer_id!= 0)
	{
		RayPacketBlock *ray_packet_block= reinterpret_cast<RayPacketBlock *>(*ray_packet_buffer_id);
		ray_packet_block->state= BlockState::Full;

		//RayPacket *foo= (RayPacket *)*ray_packets;
		//RayPacketExtras *bar= (RayPacketExtras *)*ray_packet_extras;

		shutter->ReturnRayPacketBlock(ray_packet_block);
	}

	ray_packet_block= shutter->TakeEmptyIndirectRayPacketBlock();
	*packet_count= &(ray_packet_block->front_index);
	*ray_packet_buffer_id= reinterpret_cast<intptr_t>(ray_packet_block);

	*ray_packets= (ray_packet_block->ray_packets);
	*ray_packet_extras= (ray_packet_block->ray_packet_extrass);

	RayPacket *foo= (RayPacket *)*ray_packets;
	RayPacketExtras *bar= (RayPacketExtras *)*ray_packet_extras;
}

void ReturnRayPacketBuffer(intptr_t ray_packet_buffer_id, intptr_t camera_id)
{
	RayPacketBlock *ray_packet_block= reinterpret_cast<RayPacketBlock *>(ray_packet_buffer_id);
	if(ray_packet_block->front_index> 0)
		ray_packet_block->state= ray_packet_block->front_index> (RAY_PACKET_BLOCK_SIZE* 0.9f) ? BlockState::Full : BlockState::Partial;

	if(ray_packet_block->state== BlockState::Full)
		ray_packet_block->state= BlockState::Full;

	reinterpret_cast<Camera *>(camera_id)->GetShutter()->ReturnRayPacketBlock(ray_packet_block);
}