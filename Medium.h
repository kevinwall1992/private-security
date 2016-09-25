#ifndef EBR_MEDIUM
#define EBR_MEDIUM

#include "Common.h"

struct Medium
{
	float refractive_index;
	int is_air;
};

struct MediumPacket
{
	float refractive_index[PACKET_SIZE];
	int is_air[PACKET_SIZE];
};

#endif