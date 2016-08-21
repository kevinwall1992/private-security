//May want to prefix all of these with EBR... Gross but otherwise its an unsafe naming policy

#define PACKET_SIZE 8

//Going to postpone support for more than 1 samples per pixel
#define SAMPLES_PER_PIXEL 1
#define CAMERA_TILE_WIDTH 64
#define CAMERA_TILE_HEIGHT 32

#define FILM_INTERVAL_SIZE 8192

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

//We want to have a way to test out ray block sizes and tile sizes independently
//We can at least use a multiple pretty easily
#define RAY_BLOCK_SIZE (CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT)
#define RAY_PACKET_BLOCK_SIZE (RAY_BLOCK_SIZE/ PACKET_SIZE)

#define THREAD_COUNT 4
#define SERIAL_MODE 0

#define ISPC_GET_RAYS 1
#define PARALLEL_DEVELOP 1
#define ISPC_DEVELOP 1
#define ISPC_SHADING 1
#define ISPC_CLEAR 1

#define SOA_RECEPTORS 1

#define STREAM_MODE_ 0
#define PACKET_MODE_ 1

