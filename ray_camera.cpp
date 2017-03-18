#include "RayCamera.h"

#ifdef USE_EBR

#include "ISPCKernels.h"
#include "Sampling.h"
#include "Timer.h"


void RayCameraBase::ComputeViewPlane()
{
	Vec3f right= GetRight();
	Vec3f up= GetUp();

	view_plane_u= GetRight()* tan(FOV/ 2);
	view_plane_v= GetUp()* tan(FOV/ 2);
}

RayCameraBase::RayCameraBase(float fov, Vec3f position)
	: Camera(fov, position)
{

}

Ray RayCameraBase::GetRay(float x, float y)
{
	Vec3f forward= GetForward();

	return Ray(Position, Vec3f(forward.x+ view_plane_u.x* x+ view_plane_v.x* y, forward.y+ view_plane_u.y* x+ view_plane_v.y* y, forward.z+ view_plane_u.z* x+ view_plane_v.z* y));
}

void RayCameraBase::Update()
{
	ComputeViewPlane();
}


void RayCamera::GetSamples(float *&samples_x, float *&samples_y)
{
#if PROGRESSIVE_RENDER && SAMPLE_PIXEL_CENTERS== 0
	
	//Naming needs updating, now that we have a frame_count member
	//(Used to be GraphicsSystem::frame_count)
	if(current_frame_count!= frame_count)
	{
		Turn turn= Team::WaitForTurn("get_samples_queue");
		if(current_frame_count!= frame_count)
		{
			current_frame_count= frame_count;

			for(int i= 0; i< MIN_SAMPLES_PER_PIXEL; i++)
			{
				int sample_index= current_frame_count* MIN_SAMPLES_PER_PIXEL+ i+ 20;
				progressive_samples_x[i]= HaltonSequence(2, sample_index);
				progressive_samples_y[i]= HaltonSequence(3, sample_index);
			}
		}
		turn.AllDone();
	}

	samples_x= progressive_samples_x;
	samples_y= progressive_samples_y;

	samples_x= default_samples_x;
	samples_y= default_samples_y;

#else
	samples_x= default_samples_x;
	samples_y= default_samples_y;
#endif
}

void RayCamera::Update()
{
	RayCameraBase::Update();

	film.Clear();

	frame_count= 0;
}

RayCamera::RayCamera(float fov, Vec3f position)
	: RayCameraBase(fov, position), shutter(this)
{

#if SAMPLE_PIXEL_CENTERS
	for(int i= 0; i< MIN_SAMPLES_PER_PIXEL; i++)
	{
		default_samples_x[i]= 0.5f; 
		default_samples_y[i]= 0.5f;
	}

#elif CURATED_SAMPLES

	default_samples_x[0]= 0.24f; default_samples_y[0]= 0.12f;
	default_samples_x[1]= 0.78f; default_samples_y[1]= 0.37f;
	default_samples_x[2]= 0.12f; default_samples_y[2]= 0.64f;
	default_samples_x[3]= 0.81f; default_samples_y[3]= 0.86f;

	default_samples_x[4]= 0.61f; default_samples_y[4]= 0.15f;
	default_samples_x[5]= 0.10f; default_samples_y[5]= 0.27f;
	default_samples_x[6]= 0.49f; default_samples_y[6]= 0.49f;
	default_samples_x[7]= 0.44f; default_samples_y[7]= 0.79f;

	default_samples_x[8]= 0.44f; default_samples_y[8]= 0.02f;
	default_samples_x[9]= 0.82f; default_samples_y[9]= 0.06f;
	default_samples_x[10]= 0.50f; default_samples_y[10]= 0.30f;
	default_samples_x[11]= 0.28f; default_samples_y[11]= 0.46f;
	default_samples_x[12]= 0.70f; default_samples_y[12]= 0.56f;
	default_samples_x[13]= 0.93f; default_samples_y[13]= 0.55f;
	default_samples_x[14]= 0.04f; default_samples_y[14]= 0.89f;
	default_samples_x[15]= 0.59f; default_samples_y[15]= 0.92f;

	default_samples_x[16]= 0.07f; default_samples_y[16]= 0.06f;
	default_samples_x[17]= 0.29f; default_samples_y[17]= 0.28f;
	default_samples_x[18]= 0.89f; default_samples_y[18]= 0.24f;
	default_samples_x[19]= 0.06f; default_samples_y[19]= 0.44f;
	default_samples_x[20]= 0.34f; default_samples_y[20]= 0.64f;
	default_samples_x[21]= 0.61f; default_samples_y[21]= 0.72f;
	default_samples_x[22]= 0.94f; default_samples_y[22]= 0.73f;
	default_samples_x[23]= 0.25f; default_samples_y[23]= 0.86f;

#else
	for(int i= 0; i< MAX_SAMPLES_PER_PIXEL; i++)
	{
		default_samples_x[i]= HaltonSequence(2, i);
		default_samples_y[i]= HaltonSequence(3, i);
	}

#endif

#if FILTERING
	float standard_deviation= 0.4f;
	float total_weight= 0;
	for(int i= 0; i< 9; i++)
	{
		Vec2f center(i% 3+ 0.5f- 1.0f, i/ 3+ 0.5f- 1.0f);

		for(int j= 0; j< MIN_SAMPLES_PER_PIXEL; j++)
		{
			int index= i* 8+ j;
			float distance= sqrt(pow(samples_x[j]- center.x, 2)+ 
			                     pow(samples_y[j]- center.y, 2));

#if FILTERING == GAUSSIAN_FILTERING
			filtering_kernels[index]= pow(2.72, -(pow(distance, 2.0f)/ (2.0f* pow(standard_deviation, 2.0f))))/ sqrt(2.0f* pow(standard_deviation, 2.0f)* 3.14);
#elif FILTERING == SINC_FILTERING
			distance*= 2.0f;
			kernel_weights[index]= sin(distance)/ distance;
#endif

			if(i== 4)
				filtering_kernels[index]+= 1.0f;

			total_weight+= filtering_kernels[index];
		}
		

	}
	for(int i= 0; i< MIN_SAMPLES_PER_PIXEL* 9; i++)
		filtering_kernels[i]*= MIN_SAMPLES_PER_PIXEL/ total_weight;
#endif
}

void RayCamera::LookAt(Vec3f look_at_position)
{
	Camera::LookAt(look_at_position);

	Vec3f forward= GetForward();
	Vec3f right= GetRight();
	Vec3f up= GetUp();
}

PhotoBook RayCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	if(film.Width!= size.x || film.Height!= size.y)
	{
		film.Size= size;
		film_was_resized= true;
	}
	CatchUp();

	shutter.Open(scene);

#if PROGRESSIVE_RENDER
	frame_count++;
#endif


	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::FullColor };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::FullColor: photo_book[type]= film.image; break;

			case Photo::Type::DiffuseColor:
			case Photo::Type::SpecularColor: 
			case Photo::Type::Depth: 
			case Photo::Type::Normal:
			default: break;
			}

	return photo_book;
}

bool RayCamera::GetRayPackets(EBRRayPacket *ray_packets, int tile_index, int *indices, int index_count)
{
	//if(tile_index> 963)
	//	return false;

	if((indices== nullptr) && (tile_index>= ((film.Width/ CAMERA_TILE_WIDTH)* (film.Height/ CAMERA_TILE_HEIGHT))))
		return false;

	float *samples_x;
	float * samples_y;
	GetSamples(samples_x, samples_y);

	Vec3f forward= GetForward();
	Vec3f position= Position;

	Timer::get_rays_timer.Start();

#if ISPC_GET_RAYS
	if(indices== nullptr)
	{
		int tile_count_x= film.Width/ CAMERA_TILE_WIDTH;
		int tile_x= tile_index% tile_count_x;
		int tile_y= tile_index/ tile_count_x;
		int x_offset= tile_x* CAMERA_TILE_WIDTH;
		int y_offset= tile_y* CAMERA_TILE_HEIGHT;

		ispc::GetRayPackets(x_offset, y_offset,
							reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
							reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
							film.Width, film.Height, 
							reinterpret_cast<ispc::RayPacket *>(ray_packets), 
							samples_x, samples_y, 
							RAY_PACKET_BLOCK_SIZE,
							indices);
	}
	else
	{
		//This runs faster if you put same-pixel packets adjacent to eachother, 
		//but gains aren't THAT high, and that solution is considerably more complex
		int additional_sample_set_count= ADDITIONAL_SAMPLES_PER_PIXEL/ MIN_SAMPLES_PER_PIXEL;
		for(int i= 0; i< additional_sample_set_count; i++)
		{
			int sample_offset= (i+ 1)* MIN_SAMPLES_PER_PIXEL;

			ispc::GetRayPackets(0, 0,
								reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
								reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
								film.Width, film.Height, 
								reinterpret_cast<ispc::RayPacket *>(ray_packets+ i* index_count), 
								samples_x+ sample_offset, samples_y+ sample_offset,
								index_count,
								indices);
		}
	}


#else
	//int tile_index= next_tile_index++;//will want to test this to see that it indeed works as expected
	int tile_count_x= film->width/ CAMERA_TILE_WIDTH;//May be best to compute this only once
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

	CompleteRayPacket next_ray_packet= first_ray_packet;
	for(int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		for(int i= 0; i< CAMERA_TILE_WIDTH; i+= PACKET_SIZE)
		{
			for(int k= 0; k< PACKET_SIZE; k++)
			{
				int x= i+ k+ tile_x* CAMERA_TILE_WIDTH;
				int y= j+ tile_y* CAMERA_TILE_HEIGHT;

				float normalized_x= (((x- 0)+ 0.5f)/ film->width)* 2- 1;
				float normalized_y= ((y+ 0.5f)/ film->height)* 2- 1;

				next_ray_packet.ray_packet->orgx[k]= position[0];
				next_ray_packet.ray_packet->orgy[k]= position[1];
				next_ray_packet.ray_packet->orgz[k]= position[2];

				//-normalized is quick hack
				next_ray_packet.ray_packet->dirx[k]= forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
				next_ray_packet.ray_packet->diry[k]= forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
				next_ray_packet.ray_packet->dirz[k]= forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

				next_ray_packet.ray_packet->tnear[k]= 0.0f;
				next_ray_packet.ray_packet->tfar[k]= FLT_MAX;
				next_ray_packet.ray_packet->geomID[k]= RTC_INVALID_GEOMETRY_ID;
				next_ray_packet.ray_packet->primID[k]= RTC_INVALID_GEOMETRY_ID;
				next_ray_packet.ray_packet->mask[k]= -1;
				next_ray_packet.ray_packet->time[k]= 0;

				next_ray_packet.extras->absorption_r[k]= 1.0f;
				next_ray_packet.extras->absorption_g[k]= 1.0f;
				next_ray_packet.extras->absorption_b[k]= 1.0f;

				next_ray_packet.extras->bounce_count[k]= 0;
				next_ray_packet.extras->type[k]= RayType::Primary;
				next_ray_packet.extras->x[k]= x;
				next_ray_packet.extras->y[k]= y;
			}

			next_ray_packet.ray_packet++;
			next_ray_packet.extras++;
		}
	}

#endif

	Timer::get_rays_timer.Pause();

	return true;
}

bool RayCamera::GetRays(EBRRay *rays, int tile_index, int *indices)
{
	if(tile_index>= ((film.Width/ CAMERA_TILE_WIDTH)* (film.Height/ CAMERA_TILE_HEIGHT)))
		return false;

#if ISPC_GET_RAYS
	//This has not been tested

	int tile_count_x= film.Width/ CAMERA_TILE_WIDTH;
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;
	int x_offset= tile_x* CAMERA_TILE_WIDTH;
	int y_offset= tile_y* CAMERA_TILE_HEIGHT;

	int ray_count= 0;
	for(int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		for(int i= 0; i< CAMERA_TILE_WIDTH; i++)
		{
			int x= i+ tile_x* CAMERA_TILE_WIDTH;
			int y= j+ tile_y* CAMERA_TILE_HEIGHT;
			if(x< film.Width && y< film.Height);
			else
				continue;

			int ray_index= ray_count++;

			rays[ray_index].tnear = 0.0f;
			rays[ray_index].tfar = FLT_MAX;
			rays[ray_index].geomID = RTC_INVALID_GEOMETRY_ID;
			rays[ray_index].primID = RTC_INVALID_GEOMETRY_ID;
			rays[ray_index].mask = -1;
			rays[ray_index].time = 0;

			rays[ray_index].absorption= Color(1.0f, 1.0f, 1.0f);
			rays[ray_index].bounce_count= 0;
			rays[ray_index].type= RayType::Primary;
			rays[ray_index].x= (float)x;
			rays[ray_index].y= (float)y;
		}
	}

	return true;

#else
	//int tile_index= next_tile_index++;//will want to test this to see that it indeed works as expected
	int tile_count_x= film->width/ CAMERA_TILE_WIDTH;//May be best to compute this only once
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

	CompleteRay next_ray= first_ray;
	for(int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		for(int i= 0; i< CAMERA_TILE_WIDTH; i++)
		{
			int x= i+ tile_x* CAMERA_TILE_WIDTH;
			int y= j+ tile_y* CAMERA_TILE_HEIGHT;

			float normalized_x= ((x+ 0.5f)/ film->width)* 2- 1;
			float normalized_y= ((y+ 0.5f)/ film->height)* 2- 1;

			SetFloat3(next_ray.ray->org, position);
			//-normalized is quick hack
			next_ray.ray->dir[0]= forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
			next_ray.ray->dir[1]= forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
			next_ray.ray->dir[2]= forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

			next_ray.ray->tnear = 0.0f;
			next_ray.ray->tfar = FLT_MAX;
			next_ray.ray->geomID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->primID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->mask = -1;
			next_ray.ray->time = 0;

			next_ray.extras->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.extras->bounce_count= 0;
			next_ray.extras->type= RayType::Primary;
			next_ray.extras->x= x;
			next_ray.extras->y= y;

			next_ray.ray++;
			next_ray.extras++;
		}
	}

	return true;

#endif
}

float * RayCamera::GetFilteringKernels()
{
	return filtering_kernels;
}

#endif