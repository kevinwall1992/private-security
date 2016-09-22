#include "Shutter.h"
#include "Scene.h"
#include "Camera.h"
#include "Surface.h"
#include "Timer.h"

#include "ISPCKernels.h"


template<class T>
T * PopFront(queue<T *> &ray_blocks)
{
	Turn turn= Team::WaitForTurn("ray_block_exchange_queue");

	T *ray_block= nullptr;

	if(ray_blocks.size()> 0)
	{
		ray_block= ray_blocks.front();
		ray_blocks.pop();
	}

	turn.AllDone();

	return ray_block;
}

template<class T>
T * PopFrontOrInstanciate(queue<T *> &ray_blocks, bool is_primary, bool is_coherent)
{
	T *ray_block= PopFront(ray_blocks);

	if(ray_block== nullptr)
		return new T(is_primary, is_coherent);

	return ray_block;
}


RayBlock * Shutter::TakeEmptyPrimaryRayBlock()
{
	return PopFront(empty_primary_ray_blocks);
}

RayBlock * Shutter::TakeEmptySecondaryRayBlock()
{
	return PopFrontOrInstanciate(empty_secondary_ray_blocks, false, true);
}

RayBlock * Shutter::TakePartialSecondaryRayBlock()
{
	RayBlock *ray_block= nullptr;

	while(ray_block== nullptr && empty_secondary_ray_blocks.size()> 0)
	{
		ray_block= PopFront(empty_secondary_ray_blocks);
		if(ray_block->state== BlockState::Empty)
		{
			delete ray_block;
			ray_block= nullptr;
		}
	}

	return ray_block;
}

RayBlock * Shutter::TakeFullRayBlock()
{
	return PopFront(full_ray_blocks);
}

void Shutter::ReturnRayBlock(RayBlock *ray_block)
{
	Turn turn= Team::WaitForTurn("ray_block_exchange_queue");

	if(ray_block->state== BlockState::Full)
		full_ray_blocks.push(ray_block);
	else
	{
		if(ray_block->is_primary)
			empty_primary_ray_blocks.push(ray_block);
		else
			empty_secondary_ray_blocks.push(ray_block);
	}

	turn.AllDone();
}

RayPacketBlock * Shutter::TakeEmptyPrimaryRayPacketBlock()
{
	return PopFront(empty_primary_ray_packet_blocks);
}

RayPacketBlock * Shutter::TakeEmptySecondaryRayPacketBlock()
{
	return PopFrontOrInstanciate(empty_secondary_ray_packet_blocks, false, false);
}

RayPacketBlock * Shutter::TakePartialSecondaryRayPacketBlock()
{
	RayPacketBlock *ray_packet_block= nullptr;

	while(ray_packet_block== nullptr && empty_secondary_ray_packet_blocks.size()> 0)
	{
		ray_packet_block= PopFront(empty_secondary_ray_packet_blocks);
		if(ray_packet_block!= nullptr && ray_packet_block->state== BlockState::Empty)
		{
			delete ray_packet_block;
			ray_packet_block= nullptr;
		}
	}

	return ray_packet_block;
}

RayPacketBlock * Shutter::TakeFullRayPacketBlock()
{
	return PopFront(full_ray_packet_blocks);
}

void Shutter::ReturnRayPacketBlock(RayPacketBlock *ray_packet_block)
{
	Turn turn= Team::WaitForTurn("ray_block_exchange_queue");

	if(ray_packet_block->state== BlockState::Full)
		full_ray_packet_blocks.push(ray_packet_block);
	else if(ray_packet_block->is_primary)
		empty_primary_ray_packet_blocks.push(ray_packet_block);
	else
		empty_secondary_ray_packet_blocks.push(ray_packet_block);

	turn.AllDone();
}

Shutter::Shutter(Camera *camera_)
#if SERIAL_MODE == 0
	: team(THREAD_COUNT)
#endif
{
	camera= camera_;

#if PACKET_MODE
	float ratio= sizeof(VisibilityRayPacket)/ (float)(sizeof(VisibilityRay)* PACKET_SIZE);
	ratio= std::max(ratio, 1/ ratio);
	int shadow_ray_buffer_size= (int)(RAY_PACKET_BLOCK_SIZE* ratio)+ 1;

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
	int camera_tile_index= Team::TakeANumber("next_camera_tile_index");
	bool successful= camera->GetRays(primary_ray_block->rays+ primary_ray_block->front_index, camera_tile_index);
	if(successful)
		primary_ray_block->front_index+= RAY_BLOCK_SIZE;

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

void ShadingKernel(Ray &ray, vector<Light *> &lights, vector<AmbientLight *> &ambient_lights, Film *film, Scene *scene)
{
	if(ray.geomID== RTC_INVALID_GEOMETRY_ID)
		return;

	PhongMaterial *material= dynamic_cast<PhongMaterial *>(ray.surface.material);

	Color color= Color(0, 0, 0);

	for(unsigned int i= 0; i< lights.size(); i++)
	{
		float light_coefficient= ray.light_coefficients[i];
		if(light_coefficient<= 0)
			continue;
		
		color+= lights[i]->GetLuminosity(ray.surface.position)* light_coefficient* material->diffuse;
	}

	for(unsigned int i= 0; i< ambient_lights.size(); i++)
		color+= ambient_lights[i]->GetLuminosity(ray.surface.position);

	film->Stimulate(ray.x, ray.y, color* ray.absorption);
}

void Shutter::Shade(RayBlock *ray_block, Scene *scene, Film *film)
{
	//Intersection

	Timer::embree_timer.Start();

#if STREAM_MODE || SINGLE_RAY_TEST
	scene->Intersect(ray_block->rays, RAY_BLOCK_SIZE, ray_block->is_coherent);
#else
	for(int i= 0; i< RAY_BLOCK_SIZE; i++)
		scene->Intersect(ray_block->rays[i]);
#endif

	Timer::embree_timer.Pause();


	//Interpolation

	Timer::pre_shading_timer.Start();

	for(int i= 0; i< ray_block->front_index; i++)
	{
		Ray *ray= &ray_block->rays[i];

		if(ray->geomID== RTC_INVALID_GEOMETRY_ID)
			continue;

		Prop *prop= scene->GetProp(ray->geomID);
		ray->surface.material= prop->material;

		ray->surface.position= Vec3f(ray->org[0]+ ray->dir[0]* ray->tfar, 
			                                ray->org[1]+ ray->dir[1]* ray->tfar, 
											ray->org[2]+ ray->dir[2]* ray->tfar);

		float u= ray->u;
		float v= ray->v;
		float w= 1- u- v;

		int *normal_indices= &(prop->mesh->normal_indices[ray->primID* 3]);
		Vec3f normal0= MakeVec3f(&(prop->mesh->normals[normal_indices[0]* 3]));
		Vec3f normal1= MakeVec3f(&(prop->mesh->normals[normal_indices[1]* 3]));
		Vec3f normal2= MakeVec3f(&(prop->mesh->normals[normal_indices[2]* 3]));

		ray->surface.normal= normal0* w+ normal1* u+ normal2* v;
	}

	Timer::pre_shading_timer.Pause();


	//Shadows

	vector<Light *> lights= scene->GetLights();
	vector<AmbientLight *> ambient_lights= scene->GetAmbientLights();
	float *light_coefficients= Team::GetBuffer<float>("light_coefficient_buffer", RAY_BLOCK_SIZE* lights.size());
	VisibilityRay *shadow_rays= Team::GetBuffer<VisibilityRay>("shadow_ray_buffer", RAY_BLOCK_SIZE);
	for(int i= 0; i< ray_block->front_index; i++)
		ray_block->rays[i].light_coefficients= light_coefficients+ i* lights.size();

	Timer::shadow_timer.Start();

	for(int light_index= 0; light_index< lights.size(); light_index++)
	{
		float geometry_terms[RAY_BLOCK_SIZE];
		int ray_indices[RAY_BLOCK_SIZE];

		int shadow_ray_count= 0;
		for(int i= 0; i< ray_block->front_index; i++)
		{
			Ray *ray= &ray_block->rays[i];

			if(ray->geomID== RTC_INVALID_GEOMETRY_ID)
				continue;

			Vec3f direction= -lights[light_index]->SampleDirectionAtPoint(ray->surface.position);
			float geometry_term= dot(normalize(direction), ray->surface.normal);

			if(geometry_term< 0)
			{
				ray_block->rays[i].light_coefficients[light_index]= 0.0f;
				continue;
			}

			int shadow_ray_index= shadow_ray_count++;

			ray_indices[shadow_ray_index]= i;
			geometry_terms[shadow_ray_index]= geometry_term;

			SetFloat3(shadow_rays[shadow_ray_index].org, ray->surface.position+ ray->surface.normal* 0.01f);
			SetFloat3(shadow_rays[shadow_ray_index].dir, direction);

			shadow_rays[shadow_ray_index].tnear= 0.0f;
			shadow_rays[shadow_ray_index].tfar= 1.0f;
			shadow_rays[shadow_ray_index].geomID= RTC_INVALID_GEOMETRY_ID;
			shadow_rays[shadow_ray_index].primID= RTC_INVALID_GEOMETRY_ID;
			shadow_rays[shadow_ray_index].mask= -1;
			shadow_rays[shadow_ray_index].time= 0;
		}

		scene->Intersect_Visibility(shadow_rays, shadow_ray_count, ray_block->is_coherent);

		for(int i= 0; i< shadow_ray_count; i++)
		{
			int ray_index= ray_indices[i];

			if(shadow_rays[i].geomID== 0)
				ray_block->rays[ray_index].light_coefficients[light_index]= 0.0f;
			else
				ray_block->rays[ray_index].light_coefficients[light_index]= geometry_terms[i];
		}
	}

	Timer::shadow_timer.Pause();


	//Shading

	Timer::shading_timer.Start();

	int rays_processed_count= 0;
	for(unsigned int i= 0; i< ray_block->front_index; i++)
	{
		if(ray_block->rays[i].geomID== RTC_INVALID_GEOMETRY_ID)
			continue;

		ShadingKernel(ray_block->rays[i], lights, ambient_lights, film, scene);
		rays_processed_count++;
	}

	Timer::shading_timer.Pause();

	ray_block->Empty();
	ReturnRayBlock(ray_block);

#undef light_count
}

void Shutter::Develop(Film *film)
{
	int film_interval_index= Team::TakeANumber("next_film_interval_index");
	develop_finished= !film->Develop_Parallel(film_interval_index);
	film->Clear_Parallel(film_interval_index);
}

void Shutter::PacketedRefill(RayPacketBlock *primary_ray_packet_block)
{
	int camera_tile_index= Team::TakeANumber("next_camera_tile_index");
	bool successful= camera->GetRayPackets(primary_ray_packet_block->GetFront(), camera_tile_index);
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
		int offset= Team::TakeANumber("next_noisy_receptors_interval_index")* interval_size;
		int index_count= std::min((noisy_receptors_front- offset), interval_size);
		if(offset< noisy_receptors_front)
		{
			camera->GetRayPackets(primary_ray_packet_block->GetFront(), 0, noisy_receptors+ offset, index_count);
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

#if ISPC_SHADING == 0
void PacketedShadingKernel(RayPacket &ray_packet, Film *film, Scene *scene)
{
	for(int i= 0; i< PACKET_SIZE; i++)
	{
		if(ray_packet.geomID[i]== RTC_INVALID_GEOMETRY_ID)
			continue;

		Color color= Color(0, 0, 0);

		Surface surface;
		surface.position[0]= ray_packet.orgx[i]+ (ray_packet.dirx[i]* ray_packet.tfar[i]);
		surface.position[1]= ray_packet.orgy[i]+ (ray_packet.diry[i]* ray_packet.tfar[i]);
		surface.position[2]= ray_packet.orgz[i]+ (ray_packet.dirz[i]* ray_packet.tfar[i]);
		surface.normal[0]= -ray_packet.Ngx[i];
		surface.normal[1]= -ray_packet.Ngy[i];
		surface.normal[2]= -ray_packet.Ngz[i];
		surface.normal= normalize(surface.normal);

		vector<Light *> *lights= scene->GetLights();
		for(unsigned int j= 0; j< lights->size(); j++)
		{
			float geometry_term= dot(surface.normal, normalize(-(*lights[j])->SampleDirectionAtPoint(surface.position, 0)));

			color+= (*lights[j])->GetLuminosity(surface.position)* geometry_term;
		}

		vector<AmbientLight *> *ambient_lights= scene->GetAmbientLights();
		for(unsigned int j= 0; j< ambient_lights->size(); j++)
			color+= (*ambient_lights)[j]->GetLuminosity(surface.position);

		color[0]*= ray_packet.absorption_r[i];
		color[1]*= ray_packet.absorption_g[i];
		color[2]*= ray_packet.absorption_b[i];

		film->Stimulate(ray_packet.x[i], ray_packet.y[i], color);
	}
}
#endif

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
#if ISPC_INTERPOLATION == 0
		scene->Interpolate(ray_packet_block->ray_packets[i], ray_packet_block->ray_packet_extrass[i]);
#endif
	}
#endif
	Timer::embree_timer.Pause();

	//Shadows

	//Preshading - Interpolation

	ISPCLighting *lighting= scene->GetISPCLighting();
	float *light_coefficient_buffer= Team::GetBuffer<float>("light_coefficient_buffer", RAY_PACKET_BLOCK_SIZE* PACKET_SIZE* lighting->point_light_count);
	ispc::VisibilityRayPacket *shadow_ray_packet_buffer= reinterpret_cast<ispc::VisibilityRayPacket *>(Team::GetBuffer<ispc::VisibilityRayPacket>("shadow_ray_buffer", RAY_PACKET_BLOCK_SIZE));
	for(int i= 0; i< ray_packet_block->front_index; i++)
		ray_packet_block->ray_packets[i].light_coefficients= light_coefficient_buffer+ i* PACKET_SIZE* lighting->point_light_count;

	Timer::pre_shading_timer.Start();
#if ISPC_INTERPOLATION
	ISPCMesh *meshes= scene->GetISPCMeshes();
	int *material_ids= scene->GetMaterialIDs();

	ispc::Interpolate(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets), 
					 ray_packet_block->front_index,
					 meshes, material_ids);
#endif
	Timer::pre_shading_timer.Pause();

	//Shadows
	

	Timer::shadow_timer.Start();
	ispc::ComputeLightCoefficients(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets), 
							ray_packet_block->front_index,
							lighting, 
							shadow_ray_packet_buffer, 
							reinterpret_cast<ispc::__RTCScene **>(scene->GetEmbreeScene()));
	Timer::shadow_timer.Pause();

	//Shading
	Timer::shading_timer.Start();
#if ISPC_SHADING
	ISPCMaterial *materials= scene->GetISPCMaterials();

	if(!ray_packet_block->is_additional)
	{
#if ADAPTIVE_SAMPLING
		int noisy_receptors[CAMERA_TILE_WIDTH* CAMERA_TILE_HEIGHT];
#else
		int *noise_receptors= nullptr;
#endif
		int noisy_receptors_count= 0;

#if 1
		int total_rays_shaded_count= 0;
		while(total_rays_shaded_count< ray_packet_block->front_index)
		{
#if PACKETED_SECONDARY_RAYS
			RayPacketBlock *secondary_ray_packet_block= TakeEmptySecondaryRayPacketBlock();

			int rays_shaded_count= 0;
			ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets+ total_rays_shaded_count), 
										ray_packet_block->front_index- total_rays_shaded_count, 
										&rays_shaded_count,
										film->receptors_r, film->receptors_g, film->receptors_b, 
										film->sample_counts, film->width, 
										lighting, materials, 
										camera->GetFilteringKernels(), 
										noisy_receptors, &noisy_receptors_count, 
										reinterpret_cast<ispc::RayPacket *>(secondary_ray_packet_block->ray_packets), 
										&(secondary_ray_packet_block->front_index));

			if(secondary_ray_packet_block->front_index> 0)
				secondary_ray_packet_block->state= secondary_ray_packet_block->front_index>= (RAY_PACKET_BLOCK_SIZE- 1)? BlockState::Full : BlockState::Partial;
			ReturnRayPacketBlock(secondary_ray_packet_block);

#else
			RayBlock *secondary_ray_block= TakeEmptySecondaryRayBlock();

			int rays_shaded_count= 0;
			ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets+ total_rays_shaded_count), 
										ray_packet_block->front_index- total_rays_shaded_count, 
										&rays_shaded_count,
										film->receptors_r, film->receptors_g, film->receptors_b, 
										film->sample_counts, film->width, 
										lighting, materials, 
										camera->GetFilteringKernels(), 
										noisy_receptors, &noisy_receptors_count, 
										reinterpret_cast<ispc::Ray *>(secondary_ray_block->rays), 
										&(secondary_ray_block->front_index));

			if(secondary_ray_block->front_index> 0)
				secondary_ray_block->state= secondary_ray_block->front_index>= (RAY_BLOCK_SIZE- 1* PACKET_SIZE)? BlockState::Full : BlockState::Partial;

			ReturnRayBlock(secondary_ray_block);
#endif

			total_rays_shaded_count+= rays_shaded_count;
		}

#else
		ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets), 
									reinterpret_cast<ispc::RayPacketExtras *>(ray_packet_block->ray_packet_extrass), 
									ray_packet_block->front_index, 
									film->receptors_r, film->receptors_g, film->receptors_b, film->sample_counts, 
									film->width, 
									lighting, light_coefficient_buffer, 
									materials, 
									camera->GetFilteringKernels(), 
									noisy_receptors, &noisy_receptors_count, 
									reinterpret_cast<intptr_t>(camera));

		ReportNoisyReceptors(noisy_receptors, noisy_receptors_count);
#endif
	}
	else
	{
#if 0
		ispc::PacketedShadingKernel(reinterpret_cast<ispc::RayPacket *>(ray_packet_block->ray_packets), 
									reinterpret_cast<ispc::RayPacketExtras *>(ray_packet_block->ray_packet_extrass), 
									ray_packet_block->front_index,
									film->receptors_r, film->receptors_g, film->receptors_b, film->sample_counts,
									film->width,
									lighting, light_coefficient_buffer,
									materials, 
									nullptr,
									nullptr, nullptr, 
									reinterpret_cast<intptr_t>(camera));
#endif
	}

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

	Turn turn= Team::WaitForTurn("task_queue");

#if PACKET_MODE

	RayPacketBlock *ray_packet_block= nullptr;
	RayBlock *ray_block= nullptr;

	if((!initial_samples_exhausted || !additional_samples_exhausted) && 
	   (ray_packet_block= TakeEmptyPrimaryRayPacketBlock())!= nullptr)
		task= Task(TaskType::Refill, ray_packet_block);
	else if((ray_block= TakeFullRayBlock())!= nullptr)
		task= Task(TaskType::Shade, ray_block);
	else if((ray_packet_block= TakeFullRayPacketBlock())!= nullptr)
		task= Task(TaskType::Shade, ray_packet_block);
	else if((ray_block= TakePartialSecondaryRayBlock())!= nullptr)
		task= Task(TaskType::Shade, ray_block);
	else if((ray_packet_block= TakePartialSecondaryRayPacketBlock())!= nullptr)
		task= Task(TaskType::Shade, ray_packet_block);
	else if(!develop_finished)
		task= Task(TaskType::Develop);

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

	turn.AllDone();

	return task;
}

void Shutter::Reset()
{
	initial_samples_exhausted= false;
	additional_samples_exhausted= false;
	develop_finished= false;

	if(noisy_receptors== nullptr)
		noisy_receptors= new int[camera->film->width* camera->film->height];
	noisy_receptors_front= 0;
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

	Team::WaitForOthers();

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
	Team::GetMyTeam()->Assign(&Shutter::TaskLoop, this, &scene);
#else
	team.Assign(&Shutter::TaskLoop, this, &scene);
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

Ray * RayBlock::GetFront()
{
	return rays+ front_index;
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
	//ray_packets[0].mask[0]= 7;
	//ray_packet_extrass[0].bounce_count[0]= 7;

	front_index= 0;
	state= BlockState::Empty;
}

RayPacket * RayPacketBlock::GetFront()
{
	return ray_packets+ front_index;
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