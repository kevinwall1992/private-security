
#define PACKET_SIZE 8

//Might consider renaming min as sample set size or something
//Or maybe pixels per packet maybe
#define MIN_SAMPLES_PER_PIXEL 8
#define ADDITIONAL_SAMPLES_PER_PIXEL 16
#define MAX_SAMPLES_PER_PIXEL (MIN_SAMPLES_PER_PIXEL+ ADDITIONAL_SAMPLES_PER_PIXEL)
#define CAMERA_TILE_WIDTH (64/ MIN_SAMPLES_PER_PIXEL)
#define CAMERA_TILE_HEIGHT 32

#define FILM_INTERVAL_SIZE 8192

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

//We want to have a way to test out ray block sizes and tile sizes independently
//We can at least use a multiple pretty easily
#define RAY_BLOCK_SIZE (CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT* MIN_SAMPLES_PER_PIXEL)
#define RAY_PACKET_BLOCK_SIZE (RAY_BLOCK_SIZE/ PACKET_SIZE)

#define MAX_BOUNCE_COUNT 7
#define FAINTNESS_THRESHOLD 0.2f


#define THREAD_COUNT 1
#define SERIAL_MODE (THREAD_COUNT == 1)

#define ISPC_GET_RAYS 1
#define ISPC_DEVELOP 1
#define ISPC_SHADING 1
#define ISPC_CLEAR 1

#define NO_OPENGL 1
#define DRAW_DIRECTLY_TO_SCREEN 1

#define STREAM_MODE 0
#define PACKET_MODE 1

#define ISPC_INTERPOLATION 1

#define ADAPTIVE_SAMPLING 0
#define COLOR_VARIANCE_THRESHOLD (0.05f)
#define FILTERING 0
#define SINC_FILTERING 1
#define GAUSSIAN_FILTERING 2
#define CURATED_SAMPLES 1

#define PACKETED_SECONDARY_RAYS 1


#define PRINT_AVERAGES 1

