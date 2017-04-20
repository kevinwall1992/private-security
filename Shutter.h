#ifndef EBR_SHUTTER
#define EBR_SHUTTER

#include <queue>

using std::queue;

#include "Common.h"
#include "Threading.h"
#include "EBRRay.h"
#include "Film.h"
#include "Scene.h"
#include "ISPCInterop.h"


class RayCamera;
struct RayBlock;
struct RayPacketBlock;
struct Task;

class Shutter
{
	RayCamera *camera;

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

	float *primary_disc_samples;
	int *disc_sample_indices;
	float *interval_samples;
	//int *interval_sample_order;
	int *interval_sample_indices;

	bool initial_samples_exhausted;
	bool additional_samples_exhausted;
	bool develop_finished;

	Vec2i size;


	void ReportNoisyReceptors(int *indices, int count);

	int * GetDiscSampleIndices(int x, int y, int bounce_count);
	int * GetIntervalSampleIndices(int x, int y, int bounce_count);

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
	void Resize();

public:

	int rays_processed= 0;
	int packets_processed= 0;

	Shutter(RayCamera *camera= nullptr);
	~Shutter();

	void Open(Scene &scene);
};


struct BlockState{enum Enum {Empty, Partial, Full};};

struct RayBlock
{
	EBRRay rays[RAY_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	bool is_additional;//naming

	RayBlock(bool is_primary, bool is_coherent);
	RayBlock();

	void Empty();

	EBRRay * GetFront();
};

struct RayPacketBlock
{
	EBRRayPacket ray_packets[RAY_PACKET_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	bool is_additional;//naming

	RayPacketBlock(bool is_primary, bool is_coherent);
	RayPacketBlock();

	void Empty();

	EBRRayPacket * GetFront();
};

struct TaskType{enum Enum {None, Refill, Shade, Develop, Cancel};};

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