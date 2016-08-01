#ifndef __8BR_PARAMETERS__
#define __8BR_PARAMETERS__

#define PACKET_SIZE 4

//Going to postpone support for more than 1 samples per pixel
#define SAMPLES_PER_PIXEL 1
#define TILE_WIDTH 64
#define TILE_HEIGHT 8

#define RAY_BLOCK_SIZE TILE_WIDTH* TILE_HEIGHT

#define THREAD_COUNT 1

#endif