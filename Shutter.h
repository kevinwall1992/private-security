#ifndef EBR_SHUTTER
#define EBR_SHUTTER

#include <queue>

using std::queue;

#include "Common.h"
#include "Threading.h"
#include "Ray.h"
#include "Film.h"
#include "Scene.h"
#include "ISPCInterop.h"


struct Camera;
struct RayBlock;
struct RayPacketBlock;
struct Task;

class Shutter
{
	Camera *camera;

#if SERIAL_MODE == 0
	Team team;
#endif

	queue<RayBlock *> empty_primary_ray_blocks;
	queue<RayBlock *> empty_secondary_ray_blocks; 
	queue<RayBlock *> full_ray_blocks;

	queue<RayPacketBlock *> empty_primary_ray_packet_blocks;
	queue<RayPacketBlock *> empty_secondary_ray_packet_blocks; 
	queue<RayPacketBlock *> full_ray_packet_blocks;

	int *noisy_receptors= nullptr;
	std::atomic_int noisy_receptors_front;

	bool initial_samples_exhausted;
	bool additional_samples_exhausted;
	bool develop_finished;


	void ReportNoisyReceptors(int *indices, int count);

	RayBlock * TakeEmptyPrimaryRayBlock();
	RayBlock * TakeEmptySecondaryRayBlock();
	RayBlock * TakePartialSecondaryRayBlock();
	RayBlock * TakeFullRayBlock();
	void ReturnRayBlock(RayBlock *ray_block);

	RayPacketBlock * TakeEmptyPrimaryRayPacketBlock();
	RayPacketBlock * TakeEmptySecondaryRayPacketBlock();
	RayPacketBlock * TakePartialSecondaryRayPacketBlock();
	RayPacketBlock * TakeFullRayPacketBlock();
	void ReturnRayPacketBlock(RayPacketBlock *ray_packet_block);


	void Refill(RayBlock *primary_ray_block);
	void Shade(RayBlock *ray_block, Scene *scene, Film *film);

	void PacketedRefill(RayPacketBlock *primary_ray_packet_block);
	void PacketedShade(RayPacketBlock *ray_packet_block, Scene *scene, Film *film);

	void Develop(Film *film);


	Task GetTask();
	void TaskLoop(Scene *scene);

	void Reset();

public:
	Shutter(Camera *camera= nullptr);
	~Shutter();

	void Open(Scene &scene);
};


struct BlockState{enum Enum {Empty, Partial, Full};};

struct RayBlock
{
	Ray rays[RAY_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	bool is_additional;//naming

	RayBlock(bool is_primary, bool is_coherent);
	RayBlock();

	void Empty();

	Ray * GetFront();
};

struct RayPacketBlock
{
	RayPacket ray_packets[RAY_PACKET_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	bool is_additional;//naming

	RayPacketBlock(bool is_primary, bool is_coherent);
	RayPacketBlock();

	void Empty();

	RayPacket * GetFront();
};

struct TaskType{enum Enum {None, Refill, Shade, Develop};};

struct Task
{
	TaskType::Enum type;
	bool is_packeted;

	union
	{
		struct
		{
			union
			{
				RayBlock *primary_ray_block;
				RayPacketBlock *primary_ray_packet_block;
			};
			
		}refill;

		struct
		{
			union
			{
				RayBlock *ray_block;
				RayPacketBlock *ray_packet_block;
			};
		}shade;
	};

	Task(TaskType::Enum type, RayBlock *ray_block);
	Task(TaskType::Enum type, RayPacketBlock *ray_packet_block);
	Task(TaskType::Enum type);
	Task();
};

#endif