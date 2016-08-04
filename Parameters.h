#ifndef __8BR_PARAMETERS__
#define __8BR_PARAMETERS__

#define PACKET_SIZE 4

//Going to postpone support for more than 1 samples per pixel
#define SAMPLES_PER_PIXEL 1
#define TILE_WIDTH 64
#define TILE_HEIGHT 32

#define RAY_BLOCK_SIZE (TILE_WIDTH* TILE_HEIGHT)

#define THREAD_COUNT 4
#define SERIAL_MODE 0

#define ISPC_GET_RAYS 0
#define PARALLEL_DEVELOP 1
#define ISPC_DEVELOP 1

#endif