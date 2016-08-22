
#define PACKET_SIZE 8

#define SAMPLES_PER_PIXEL 8
#define CAMERA_TILE_WIDTH (64/ SAMPLES_PER_PIXEL)
#define CAMERA_TILE_HEIGHT 32

#define FILM_INTERVAL_SIZE 8192

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

//We want to have a way to test out ray block sizes and tile sizes independently
//We can at least use a multiple pretty easily
#define RAY_BLOCK_SIZE (CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT* SAMPLES_PER_PIXEL)
#define RAY_PACKET_BLOCK_SIZE (RAY_BLOCK_SIZE/ PACKET_SIZE)

#define THREAD_COUNT 1
#define SERIAL_MODE 1

#define ISPC_GET_RAYS 1
#define ISPC_DEVELOP 1
#define ISPC_SHADING 1
#define ISPC_CLEAR 1

#define SOA_RECEPTORS 1

#define NO_OPENGL 1
#define DRAW_DIRECTLY_TO_SCREEN 1

#define STREAM_MODE 0
#define PACKET_MODE 1

