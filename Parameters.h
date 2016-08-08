#ifndef __8BR_PARAMETERS__
#define __8BR_PARAMETERS__

#define PACKET_SIZE 4

//Going to postpone support for more than 1 samples per pixel
#define SAMPLES_PER_PIXEL 1
#define CAMERA_TILE_WIDTH 64
#define CAMERA_TILE_HEIGHT 32

#define FILM_INTERVAL_SIZE 8192

//We want to have a way to test out ray block sizes and tile sizes independently
//We can at least use a multiple pretty easily
#define RAY_BLOCK_SIZE (CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT)

#define THREAD_COUNT 1
#define SERIAL_MODE 1

#define ISPC_GET_RAYS 0//fix this
#define PARALLEL_DEVELOP 1
#define ISPC_DEVELOP 1

#endif