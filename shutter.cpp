#include "Shutter.h"
#include "Scene.h"
#include "Camera.h"
#include "Surface.h"


template<class T>
T * PopFront(queue<T *> &ray_blocks, std::mutex &mutex)
{
	mutex.lock();

	T *ray_block= nullptr;

	if(ray_blocks.size()> 0)
	{
		ray_block= ray_blocks.front();
		ray_blocks.pop();
	}

	mutex.unlock();

	return ray_block;
}

template<class T>
T * PopFrontOrInstanciate(queue<T *> &ray_blocks, std::mutex &mutex, bool is_primary, bool is_coherent)
{
	T *ray_block= PopFront(ray_blocks, mutex);

	if(ray_block== nullptr)
		return new RayBlock(is_primary, is_coherent);

	return ray_block;
}


RayBlock * Shutter::TakeEmptyPrimaryRayBlock()
{
	return PopFront(empty_primary_ray_blocks, resource_mutex);
}

RayBlock * Shutter::TakeEmptyCoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_coherent_ray_blocks, resource_mutex, false, true);
}

RayBlock * Shutter::TakeEmptyIncoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_incoherent_ray_blocks, resource_mutex, false, false);
}

RayBlock * Shutter::TakeFullRayBlock()
{
	return PopFront(full_ray_blocks, resource_mutex);
}

void Shutter::ReturnRayBlock(RayBlock *ray_block)
{
	resource_mutex.lock();

	if(ray_block->state== BlockState::Full)
		full_ray_blocks.push(ray_block);
	else
	{
		if(ray_block->is_primary)
			empty_primary_ray_blocks.push(ray_block);
		else if(ray_block->is_coherent)
			empty_coherent_ray_blocks.push(ray_block);
		else
			empty_incoherent_ray_blocks.push(ray_block);
	}

	resource_mutex.unlock();
}

RayPacketBlock * Shutter::TakeEmptyPrimaryRayPacketBlock()
{
	return PopFront(empty_primary_ray_packet_blocks, resource_mutex);
}

RayPacketBlock * Shutter::TakeFullRayPacketBlock()
{
	return PopFront(full_ray_packet_blocks, resource_mutex);
}

void Shutter::ReturnRayPacketBlock(RayPacketBlock *ray_packet_block)
{
	resource_mutex.lock();

	if(ray_packet_block->state== BlockState::Full)
		full_ray_packet_blocks.push(ray_packet_block);
	else
		empty_primary_ray_packet_blocks.push(ray_packet_block);

	resource_mutex.unlock();
}

Shutter::Shutter(Camera *camera_)
{
	camera= camera_;

	Reset();

#if PACKET_MODE_
	for(int i= 0; i< THREAD_COUNT; i++)
		empty_primary_ray_packet_blocks.push(new RayPacketBlock(true, true));

#else
	for(int i= 0; i< THREAD_COUNT; i++)
		empty_primary_ray_blocks.push(new RayBlock(true, true));

#endif
}

void Shutter::Refill(RayBlock *primary_ray_block)
{
	CompleteRay complete_ray= CompleteRay(primary_ray_block->rays+ primary_ray_block->front_index, 
		                                  primary_ray_block->ray_extrass+ primary_ray_block->front_index);
	int camera_tile_index= next_camera_tile_index++;
	bool successful= camera->GetRays(complete_ray, camera_tile_index);
	if(successful)
		primary_ray_block->front_index+= RAY_BLOCK_SIZE;//assumes blocks fit perfectly

	primary_ray_block->is_coherent= true;
	if(primary_ray_block->front_index== 0)
	{
		primary_ray_block->state= BlockState::Empty;
		ray_source_exhausted= true;
	}
	if(primary_ray_block->front_index< (RAY_BLOCK_SIZE- 1))
		primary_ray_block->state= BlockState::Partial;
	else
		primary_ray_block->state= BlockState::Full;

	ReturnRayBlock(primary_ray_block);
}

void ShadingKernel(CompleteRay &ray, Film *film, Scene *scene)
{
	int x= ray.extras->x;
	int y= ray.extras->y;

	if(ray.ray->geomID== RTC_INVALID_GEOMETRY_ID)
		return;

	Color color= Color(0, 0, 0);

	Surface surface(MakeVec3f(ray.ray->org)+ (MakeVec3f(ray.ray->dir)* ray.ray->tfar), normalize(-MakeVec3f(ray.ray->Ng)));

	vector<Light *> *lights= scene->GetLights();
	for(unsigned int i= 0; i< lights->size(); i++)
	{
		//geometry term is now wrapped in occlusions
		float geometry_term= dot(surface.normal, normalize(-(*lights)[i]->SampleDirectionAtPoint(surface.position, 0)));
		//float foo_term= occlusions[i]* geometry_term;
		float foo_term= geometry_term;
		if(foo_term<= 0)
			continue;
		
		color+= (*lights)[i]->GetLuminosity(surface.position)* foo_term;
	}

	vector<AmbientLight *> *ambient_lights= scene->GetAmbientLights();
	for(unsigned int i= 0; i< ambient_lights->size(); i++)
		color+= (*ambient_lights)[i]->GetLuminosity(surface.position);

	film->Stimulate(ray.extras->x, ray.extras->y, color* ray.extras->absorption);
}

//We are currently computing occlusions for failed hits...
void Shutter::Shade(RayBlock *ray_block, Scene *scene, Film *film)
{
#if STREAM_MODE_
	scene->Intersect(ray_block->rays, RAY_BLOCK_SIZE, ray_block->is_coherent);
#else
	for(int i= 0; i< RAY_BLOCK_SIZE; i++)
		scene->Intersect(ray_block->rays[i]);
#endif

	int rays_processed_count= 0;
	for(unsigned int i= 0; i< ray_block->front_index; i++)
	{
		if(ray_block->rays[i].geomID== RTC_INVALID_GEOMETRY_ID)
			continue;

		ShadingKernel(CompleteRay(ray_block->rays+ i, ray_block->ray_extrass+ i), film, scene);
		rays_processed_count++;
	}

	ray_block->Empty();
	ReturnRayBlock(ray_block);

#undef light_count
}

void Shutter::Develop(Film *film)
{
	int film_interval_index= next_film_interval_index++;
	develop_finished= !film->Develop_Parallel(film_interval_index);
}

void Shutter::PacketedRefill(RayPacketBlock *primary_ray_packet_block)
{
	CompleteRayPacket complete_ray_packet= CompleteRayPacket(primary_ray_packet_block->ray_packets+ primary_ray_packet_block->front_index, 
		                                              primary_ray_packet_block->ray_packet_extrass+ primary_ray_packet_block->front_index);
	int camera_tile_index= next_camera_tile_index++;
	bool successful= camera->GetRayPackets(complete_ray_packet, camera_tile_index);
	if(successful)
		primary_ray_packet_block->front_index+= RAY_PACKET_BLOCK_SIZE;//assumes blocks fit perfectly

	primary_ray_packet_block->is_coherent= true;
	if(primary_ray_packet_block->front_index== 0)
	{
		primary_ray_packet_block->state= BlockState::Empty;
		ray_source_exhausted= true;
	}
	if(primary_ray_packet_block->front_index< (RAY_PACKET_BLOCK_SIZE- 1))
		primary_ray_packet_block->state= BlockState::Partial;
	else
		primary_ray_packet_block->state= BlockState::Full;

	ReturnRayPacketBlock(primary_ray_packet_block);
}

//There are two levels to the SoAness here...
//You have each member put into an array, AND you have each component
//Of the vector members put into a separate array. Need to think of how to
//Handle that second situation efficiently
void PacketedShadingKernel(CompleteRayPacket &ray_packet, Film *film, Scene *scene)
{
	for(int i= 0; i< PACKET_SIZE; i++)
	{
		int x= ray_packet.extras->x[i];
		int y= ray_packet.extras->y[i];

		if(ray_packet.ray_packet->geomID[i]== RTC_INVALID_GEOMETRY_ID)
			continue;

		Color color= Color(0, 0, 0);

		Surface surface;
		surface.position[0]= ray_packet.ray_packet->orgx[i]+ (ray_packet.ray_packet->dirx[i]* ray_packet.ray_packet->tfar[i]);
		surface.position[1]= ray_packet.ray_packet->orgy[i]+ (ray_packet.ray_packet->diry[i]* ray_packet.ray_packet->tfar[i]);
		surface.position[2]= ray_packet.ray_packet->orgz[i]+ (ray_packet.ray_packet->dirz[i]* ray_packet.ray_packet->tfar[i]);
		surface.normal[0]= -ray_packet.ray_packet->Ngx[i];
		surface.normal[1]= -ray_packet.ray_packet->Ngy[i];
		surface.normal[2]= -ray_packet.ray_packet->Ngz[i];
		surface.normal= normalize(surface.normal);

		vector<Light *> *lights= scene->GetLights();
		for(unsigned int j= 0; j< lights->size(); j++)
		{
			//geometry term is now wrapped in occlusions
			float geometry_term= dot(surface.normal, normalize(-(*lights)[j]->SampleDirectionAtPoint(surface.position, 0)));
			//float foo_term= occlusions[i]* geometry_term;
			float foo_term= geometry_term;
			if(foo_term<= 0)
				continue;
		
			color+= (*lights)[j]->GetLuminosity(surface.position)* foo_term;
		}

		vector<AmbientLight *> *ambient_lights= scene->GetAmbientLights();
		for(unsigned int j= 0; j< ambient_lights->size(); j++)
			color+= (*ambient_lights)[j]->GetLuminosity(surface.position);

		film->Stimulate(ray_packet.extras->x[i], ray_packet.extras->y[i], color* ray_packet.extras->absorption[i]);
	}
}

void Shutter::PacketedShade(RayPacketBlock *ray_packet_block, Scene *scene, Film *film)
{
#if STREAM_MODE_
	scene->Intersect(ray_packet_block->ray_packets, RAY_PACKET_BLOCK_SIZE, ray_packet_block->is_coherent);
#else
	for(int i= 0; i< RAY_PACKET_BLOCK_SIZE; i++)
		scene->Intersect(ray_packet_block->ray_packets[i]);
#endif

	int ray_packets_processed_count= 0;
	for(unsigned int i= 0; i< ray_packet_block->front_index; i++)
	{
		//if(ray_packet_block->ray_packets[i].geomID== RTC_INVALID_GEOMETRY_ID)
		//	continue;

		PacketedShadingKernel(CompleteRayPacket(ray_packet_block->ray_packets+ i, ray_packet_block->ray_packet_extrass+ i), film, scene);
		ray_packets_processed_count++;
	}

	ray_packet_block->Empty();
	ReturnRayPacketBlock(ray_packet_block);

#undef light_count
}

Task Shutter::GetTask()
{
	Task task;

	task_mutex.lock();//Test out removing this mutex after redesign finished

#if PACKET_MODE_
	RayPacketBlock *primary_ray_block= nullptr;
	if(!ray_source_exhausted)
		primary_ray_block= TakeEmptyPrimaryRayPacketBlock();
	if(primary_ray_block!= nullptr)
		task= Task(TaskType::Refill, primary_ray_block);
	else
	{
		RayPacketBlock *ray_block= TakeFullRayPacketBlock();

		if(ray_block!= nullptr)
			task= Task(TaskType::Shade, ray_block);
#if PARALLEL_DEVELOP
		else 
		{
			if(!develop_finished)
				task= Task(TaskType::Develop);
		}
#endif
	}

#else
	RayBlock *primary_ray_block= nullptr;
	if(!ray_source_exhausted)
		primary_ray_block= TakeEmptyPrimaryRayBlock();
	if(primary_ray_block!= nullptr)
		task= Task(TaskType::Refill, primary_ray_block);
	else
	{
		RayBlock *ray_block= TakeFullRayBlock();

		if(ray_block!= nullptr)
			task= Task(TaskType::Shade, ray_block);
#if PARALLEL_DEVELOP
		else 
		{
			if(!develop_finished)
				task= Task(TaskType::Develop);
		}
#endif
	}
#endif

	task_mutex.unlock();

	return task;
}

void Shutter::Reset()
{
	ray_source_exhausted= false;
	develop_finished= false;

	next_camera_tile_index= 0;
	next_film_interval_index= 0;

	barrier.Reset();
}

void Shutter::TaskLoop(Scene *scene)
{
	Task task;
	while(task.type!= TaskType::Develop)//Hack, but will be replaced in the future anyways. 
	{
		task= GetTask();

		switch(task.type)
		{
		case TaskType::Refill:
			if(task.is_packeted)
				PacketedRefill(task.refill.primary_ray_packet_block);
			else
				Refill(task.refill.primary_ray_block);
			break;
		case TaskType::Shade: 
			if(task.is_packeted)
				PacketedShade(task.shade.ray_packet_block, scene, camera->film);
			else
				Shade(task.shade.ray_block, scene, camera->film);
			break;

		default: break;
		}
	}

	barrier.Wait();

	while(true)
	{
		task= GetTask();

		switch(task.type)
		{
		case TaskType::Develop:
			Develop(camera->film);
			break;

		case TaskType::None:
			return;

		default: break;
		}
	}
}

void Shutter::Open(Scene &scene)
{
	Reset();

#if SERIAL_MODE
	TaskLoop(&scene);

#else
	for(int i= 0; i< THREAD_COUNT; i++)
		threads[i]= std::thread(&Shutter::TaskLoop, this, &scene);
	for(int i= 0; i< THREAD_COUNT; i++)
		threads[i].join();

#endif
}


RayBlock::RayBlock(bool is_primary_, bool is_coherent_)
{
	is_primary= is_primary_;
	is_coherent= is_coherent_;

	Empty();
}

RayBlock::RayBlock()
{

}

void RayBlock::Empty()
{
	front_index= 0;
	state= BlockState::Empty;
}

RayPacketBlock::RayPacketBlock(bool is_primary_, bool is_coherent_)
{
	is_primary= is_primary_;
	is_coherent= is_coherent_;

	Empty();
}

RayPacketBlock::RayPacketBlock()
{

}

void RayPacketBlock::Empty()
{
	front_index= 0;
	state= BlockState::Empty;
}


Task::Task(TaskType::Enum type_, RayBlock *ray_block)
{
	type= type_;
	is_packeted= false;

	refill.primary_ray_block= nullptr;

	switch(type)
	{
	case TaskType::Refill: 
		refill.primary_ray_block= ray_block; 
		break;

	case TaskType::Shade:
		shade.ray_block= ray_block;
		break;

	default:
		break;
	}
}

Task::Task(TaskType::Enum type_, RayPacketBlock *ray_packet_block)
{
	type= type_;
	is_packeted= true;

	refill.primary_ray_block= nullptr;

	switch(type)
	{
	case TaskType::Refill: 
		refill.primary_ray_packet_block= ray_packet_block; 
		break;

	case TaskType::Shade:
		shade.ray_packet_block= ray_packet_block;
		break;

	default:
		break;
	}
}

Task::Task(TaskType::Enum type_)
{
	type= type_;

	is_packeted= false;
	refill.primary_ray_block= nullptr;
}

Task::Task()
{
	type= TaskType::None;
	refill.primary_ray_block= nullptr;
}