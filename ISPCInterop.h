#ifndef EBR_ISPC_INTEROP
#define EBR_ISPC_INTEROP

#ifndef ISPC_CODE
#include <crtdefs.h>
extern "C" void TakeRayPacketBuffer(intptr_t *ray_packet_buffer_id, int **packet_count, intptr_t camera_id, void **ray_packets, void **ray_packet_extras);
extern "C" void ReturnRayPacketBuffer(intptr_t ray_packet_buffer_id, intptr_t camera_id);

#else
extern "C" void TakeRayPacketBuffer(intptr_t * uniform ray_packet_buffer_id, int ** uniform packet_count, uniform intptr_t camera_id, RayPacket_ ** uniform ray_packets, RayPacketExtras ** uniform ray_packet_extras);
extern "C" void ReturnRayPacketBuffer(uniform intptr_t ray_packet_buffer_id, uniform intptr_t camera_id);

#endif

#endif