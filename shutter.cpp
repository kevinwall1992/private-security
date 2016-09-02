#include "Shutter.h"
#include "Scene.h"
#include "Camera.h"
#include "Surface.h"
#include "Timer.h"

#include "ISPCKernels.h"


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

#if PACKET_MODE
	for(int i= 0; i< THREAD_COUNT; i++)
		empty_primary_ray_packet_blocks.push(new RayPacketBlock(true, true));

#else
	for(int i= 0; i< THREAD_COUNT; i++)
		empty_primary_ray_blocks.push(new RayBlock(true, true));

#endif

}

Shutter::~Shutter()
{
#if PACKET_MODE
	while(!empty_primary_ray_packet_blocks.empty())
	{
		delete empty_primary_ray_packet_blocks.front();
		empty_primary_ray_packet_blocks.pop();
	}

#else
	while(!empty_primary_ray_packet_blocks.empty())
	{
		delete empty_primary_ray_blocks.front();
		empty_primary_ray_blocks.pop();
	}

#endif

	if(noisy_receptors!= nullptr)
		 delete noisy_receptors;
}

void Shutter::ReportNoisyReceptors(int *indices, int count)
{
	if(count== 0)
		return;

	int first= std::atomic_fetch_add(&noisy_receptors_front, count);
	memcpy(noisy_receptors+ first, indices, sizeof(int)* count);//Could try this with ispc
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
		initial_samples_exhausted= true;
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
#if STREAM_MODE
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
	film->Clear_Parallel(film_interval_index);
}

void Shutter::PacketedRefill(RayPacketBlock *primary_ray_packet_block)
{
	CompleteRayPacket complete_ray_packet= CompleteRayPacket(primary_ray_packet_block->ray_packets+ primary_ray_packet_block->front_index, 
		                                              primary_ray_packet_block->ray_packet_extrass+ primary_ray_packet_block->front_index);
	int camera_tile_index= next_camera_tile_index++;
	bool successful= camera->GetRayPackets(complete_ray_packet, camera_tile_index);
	if(successful)
	{
		primary_ray_packet_block->front_index+= RAY_PACKET_BLOCK_SIZE;
		primary_ray_packet_block->is_additional= false;
	}

	primary_ray_packet_block->is_coherent= true;
	if(primary_ray_packet_block->front_index== 0)
	{
		primary_ray_packet_block->state= BlockState::Empty;
		initial_samples_exhausted= true;

		int sample_set_count= ADDITIONAL_SAMPLES_PER_PIXEL/ MIN_SAMPLES_PER_PIXEL;
		int interval_size= RAY_PACKET_BLOCK_SIZE/ sample_set_count;
		int offset= std::atomic_fetch_add(&next_noisy_receptors_interval_index, 1)* interval_size;
		int index_count= std::min((noisy_receptors_front- offset), interval_size);
		if(offset< noisy_receptors_front)
		{
			camera->GetRayPackets(complete_ray_packet, 0, noisy_receptors+ offset, index_count);
			primary_ray_packet_block->front_index+= index_count* sample_set_count;
			primary_ray_packet_block->state= BlockState::Full;
			primary_ray_packet_block->is_additional= true;
		}
		else
			additional_samples_exhausted= true;
	}
	else if(primary_ray_packet_block->front_index< (RAY_PACKET_BLOCK_SIZE- 1))
		primary_ray_packet_block->state= BlockState::Partial;
	else
		primary_ray_packet_block->state= BlockState::Full;

	ReturnRayPacketBlock(primary_ray_packet_block);
}

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

		color[0]*= ray_packet.extras->absorption_r[i];
		color[1]*= ray_packet.extras->absorption_g[i];
		color[2]*= ray_packet.extras->absorption_b[i];

		film->Stimulate(ray_packet.extras->x[i], ray_packet.extras->y[i], color);
	}
}

void Shutter::PacketedShade(RayPacketBlock *ray_packet_block, Scene *scene, Film *film)
{
	//Intersection
	Timer::embree_timer.Start();
#if STREAM_MODE
	scene->Intersect(ray_packet_block->ray_packets, RAY_PACKET_BLOCK_SIZE, ray_packet_block->is_coherent);
#else

	for(int i= 0; i< ray_packet_block->front_index; i++)
	{
		scene->Intersect(ray_packet_block->ray_packets[i]);

		//Preshading - Interpolation
		//Cannot time because too deep in loop
		//Really slow anyways, keeping for testing derivative solutions in the future
#if ISPC_INTERPOLATION == 0
		scene->Interpolate(ray_packet_block->ray_packets[i], ray_packet_block->ray_packet_extrass[i]);
#endif
	}
#endif
	Timer::embree_timer.Pause();

	//Shadows


	//Preshading - Interpolation
	Timer::pre_shading_timer.Start();
#if ISPC_INTERPOLATION
	Mesh *mesh= scene->GetMesh(0);
	ispc::Interpolate(reinterpret_cast<ispc::RayPacket_ *>(ray_packet_block->ray_packets), 
		             reinterpret_cast<ispc::RayPacketExtras *>(ray_packet_block->ray_packet_extrass), 
					 &(mesh->normal_indices[0]), 
					 &(mesh->normals[0]));
#endif
	Timer::pre_shading_timer.Pause();


	//Shading
	Timer::shading_timer.Start();
#if ISPC_SHADING
	ISPCLighting *lighting= scene->GetISPCLighting();

	if(!ray_packet_block->is_additional)
	{
		int noisy_receptors[CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT];
		int noisy_receptors_count= 0;

		ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket_ *>(ray_packet_block->ray_packets), 
									reinterpret_cast<ispc::RayPacketExtras *>(ray_packet_block->ray_packet_extrass), 
									ray_packet_block->front_index,
									film->receptors_r, film->receptors_g, film->receptors_b, film->sample_counts,
									film->width,
									lighting, 
									camera->GetFilteringKernels(),
									noisy_receptors, &noisy_receptors_count);

		ReportNoisyReceptors(noisy_receptors, noisy_receptors_count);
	}
	else
	{
		ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket_ *>(ray_packet_block->ray_packets), 
									reinterpret_cast<ispc::RayPacketExtras *>(ray_packet_block->ray_packet_extrass), 
									ray_packet_block->front_index,
									film->receptors_r, film->receptors_g, film->receptors_b, film->sample_counts,
									film->width,
									lighting, 
									nullptr,
									nullptr, nullptr);
	}

	delete lighting;

#else
	int ray_packets_processed_count= 0;
	for(unsigned int i= 0; i< ray_packet_block->front_index; i++)
	{
		PacketedShadingKernel(CompleteRayPacket(ray_packet_block->ray_packets+ i, ray_packet_block->ray_packet_extrass+ i), film, scene);
		ray_packets_processed_count++;
	}

#endif
	Timer::shading_timer.Pause();

	ray_packet_block->Empty();
	ReturnRayPacketBlock(ray_packet_block);

#undef light_count
}

Task Shutter::GetTask()
{
	Task task;

	task_mutex.lock();//Test out removing this mutex after redesign finished

#if PACKET_MODE
	RayPacketBlock *primary_ray_block= nullptr;
	if(!initial_samples_exhausted || !additional_samples_exhausted)
		primary_ray_block= TakeEmptyPrimaryRayPacketBlock();
	if(primary_ray_block!= nullptr)
		task= Task(TaskType::Refill, primary_ray_block);
	else
	{
		RayPacketBlock *ray_block= TakeFullRayPacketBlock();

		if(ray_block!= nullptr)
			task= Task(TaskType::Shade, ray_block);
		else 
		{
			if(!develop_finished)
				task= Task(TaskType::Develop);
		}
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
		else 
		{
			if(!develop_finished)
				task= Task(TaskType::Develop);
		}
	}
#endif

	task_mutex.unlock();

	return task;
}

void Shutter::Reset()
{
	initial_samples_exhausted= false;
	additional_samples_exhausted= false;
	develop_finished= false;

	next_camera_tile_index= 0;
	next_film_interval_index= 0;

	barrier.Reset();

	if(noisy_receptors== nullptr)
		noisy_receptors= new int[camera->film->width* camera->film->height];
	noisy_receptors_front= 0;
	next_noisy_receptors_interval_index= 0;
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
			//This is getting ridiculous...
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
	is_additional= false;

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
	is_additional= false;

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