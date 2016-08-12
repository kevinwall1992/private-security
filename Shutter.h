#ifndef EBR_SHUTTER
#define EBR_SHUTTER

#include <queue>

using std::queue;

#include "Common.h"
#include "Threading.h"
#include "Ray.h"
#include "Film.h"
#include "Scene.h"


struct Camera;
struct RayBlock;
struct Task;

class Shutter
{
	Camera *camera;

	queue<RayBlock *> empty_primary_ray_blocks; 
	queue<RayBlock *> empty_coherent_ray_blocks; 
	queue<RayBlock *> empty_incoherent_ray_blocks; 
	queue<RayBlock *> full_ray_blocks;

	std::thread threads[THREAD_COUNT];
	
	std::atomic_int next_camera_tile_index;
	std::atomic_int next_film_interval_index;

	bool ray_source_exhausted;
	bool develop_finished;

	std::mutex task_mutex;
	std::mutex resource_mutex;

	Barrier barrier;

protected:
	RayBlock * TakeEmptyPrimaryRayBlock();
	RayBlock * TakeEmptyCoherentRayBlock();
	RayBlock * TakeEmptyIncoherentRayBlock();
	RayBlock * TakeFullRayBlock();
	void ReturnRayBlock(RayBlock *ray_block);

	void Refill(RayBlock *primary_ray_block);
	void Shade(RayBlock *ray_block, Scene *scene, Film *film);
	void Develop(Film *film);

	Task GetTask();
	void TaskLoop(Scene *scene);

	void Reset();

public:
	Shutter(Camera *camera= nullptr);

	void Open(Scene &scene);
};


struct BlockState{enum Enum {Empty, Partial, Full};};

struct RayBlock
{
	Ray rays[RAY_BLOCK_SIZE];
	RayExtras ray_extrass[RAY_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	//vector<float> occlusion;
	

	RayBlock(bool is_primary, bool is_coherent);
	RayBlock();

	void Empty();
};

/*#define RAY_PACKET_BLOCK_SIZE (128/ PACKET_SIZE)
struct RayPacketBlock
{
	RayPacket ray_packets[RAY_BLOCK_SIZE];
	RayExtras ray_extrass[RAY_BLOCK_SIZE];
	int front_index;
	//vector<float> occlusion;

	RayPacketBlock();
};*/

/*#define SHADOW_BLOCKS_PER_RAY_BLOCK 1
#define SHADOW_BLOCK_SIZE (RAY_BLOCK_SIZE/ SHADOW_BLOCKS_PER_RAY_BLOCK)
struct ShadowBlock
{
	Ray *hits;
	RayBlock *ray_block;
};*/


struct TaskType{enum Enum {None, Refill, Shade, Develop};};

struct Task
{
	TaskType::Enum type;

	union
	{
		struct
		{
			RayBlock *primary_ray_block;
		}refill;

		struct
		{
			RayBlock *ray_block;
		}shade;
	};

	Task(TaskType::Enum type, RayBlock *ray_block= nullptr);
	Task();
};

#endif