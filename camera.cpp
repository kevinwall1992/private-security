#include "Camera.h"
#include "ISPCKernels.h"
#include "Sampling.h"
#include "Timer.h"


Camera::Camera(float fov_in_degrees, Vec3f position_, Vec3f direction_)
	: shutter(this)
{
	fov= 3.14f* (fov_in_degrees/ 180.0f);
	position= position_;
	forward= direction_;

	ComputeViewPlane();

#if CURATED_SAMPLES

	samples_x[0]= 0.24f; samples_y[0]= 0.12f;
	samples_x[1]= 0.78f; samples_y[1]= 0.37f;
	samples_x[2]= 0.12f; samples_y[2]= 0.64f;
	samples_x[3]= 0.81f; samples_y[3]= 0.86f;

	samples_x[4]= 0.61f; samples_y[4]= 0.15f;
	samples_x[5]= 0.10f; samples_y[5]= 0.27f;
	samples_x[6]= 0.49f; samples_y[6]= 0.49f;
	samples_x[7]= 0.44f; samples_y[7]= 0.79f;

	samples_x[8]= 0.44f; samples_y[8]= 0.02f;
	samples_x[9]= 0.82f; samples_y[9]= 0.06f;
	samples_x[10]= 0.50f; samples_y[10]= 0.30f;
	samples_x[11]= 0.28f; samples_y[11]= 0.46f;
	samples_x[12]= 0.70f; samples_y[12]= 0.56f;
	samples_x[13]= 0.93f; samples_y[13]= 0.55f;
	samples_x[14]= 0.04f; samples_y[14]= 0.89f;
	samples_x[15]= 0.59f; samples_y[15]= 0.92f;

	samples_x[16]= 0.07f; samples_y[16]= 0.06f;
	samples_x[17]= 0.29f; samples_y[17]= 0.28f;
	samples_x[18]= 0.89f; samples_y[18]= 0.24f;
	samples_x[19]= 0.06f; samples_y[19]= 0.44f;
	samples_x[20]= 0.34f; samples_y[20]= 0.64f;
	samples_x[21]= 0.61f; samples_y[21]= 0.72f;
	samples_x[22]= 0.94f; samples_y[22]= 0.73f;
	samples_x[23]= 0.25f; samples_y[23]= 0.86f;

#else
	for(int i= 0; i< MAX_SAMPLES_PER_PIXEL; i++)
	{
		samples_x[i]= HaltonSequence(2, i);
		samples_y[i]= HaltonSequence(3, i);
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

void Camera::LookAt(Vec3f look_at_position)
{
	forward= normalize(look_at_position- position);
	ComputeViewPlane();
}

void Camera::OrthogonalizeVectors()
{
	up= Vec3f(0, 1, 0);
	forward= normalize(forward);

	right= cross(forward, up);
	up= cross(right, forward);
}

void Camera::ComputeViewPlane()
{
	OrthogonalizeVectors();

	view_plane_u= right* tan(fov/ 2);
	view_plane_v= up* tan(fov/ 2);
}

Film * Camera::LoadFilm(Film *film_)
{
	Film *previous_film= film;
	film= film_;

	return previous_film;
}

Film * Camera::RemoveFilm()
{
	return LoadFilm(nullptr);
}

Image * Camera::TakePicture(Scene &scene)
{
	shutter.Open(scene);

	return &(film->image);
}

bool Camera::GetRayPackets(CompleteRayPacket first_ray_packet, int tile_index, int *indices, int index_count)
{
	if((indices== nullptr) && (tile_index>= ((film->width/ CAMERA_TILE_WIDTH)* (film->height/ CAMERA_TILE_HEIGHT))))
		return false;

	Timer::get_rays_timer.Start();

#if ISPC_GET_RAYS
	if(indices== nullptr)
	{
		int tile_count_x= film->width/ CAMERA_TILE_WIDTH;
		int tile_x= tile_index% tile_count_x;
		int tile_y= tile_index/ tile_count_x;
		int x_offset= tile_x* CAMERA_TILE_WIDTH;
		int y_offset= tile_y* CAMERA_TILE_HEIGHT;

		ispc::GetRayPackets(x_offset, y_offset,
							reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
							reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
							film->width, film->height, 
							reinterpret_cast<ispc::RayPacket_ *>(first_ray_packet.ray_packet), 
							reinterpret_cast<ispc::RayPacketExtras *>(first_ray_packet.extras), 
							samples_x, samples_y, 
							RAY_PACKET_BLOCK_SIZE,
							indices);
	}
	else
	{
		//This runs faster if you put same-pixel packets adjacent to eachother, 
		//but gains are THAT high, and that solution is considerably more complex
		int additional_sample_set_count= ADDITIONAL_SAMPLES_PER_PIXEL/ MIN_SAMPLES_PER_PIXEL;
		for(int i= 0; i< additional_sample_set_count; i++)
		{
			int sample_offset= (i+ 1)* MIN_SAMPLES_PER_PIXEL;

			ispc::GetRayPackets(0, 0,
								reinterpret_cast<float *>(&position), reinterpret_cast<float *>(&forward), 
								reinterpret_cast<float *>(&view_plane_u), reinterpret_cast<float *>(&view_plane_v),
								film->width, film->height, 
								reinterpret_cast<ispc::RayPacket_ *>(first_ray_packet.ray_packet+ i* index_count), 
								reinterpret_cast<ispc::RayPacketExtras *>(first_ray_packet.extras+ i* index_count), 
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

				next_ray_packet.ray_packet->tnear[k] = 0.0f;
				next_ray_packet.ray_packet->tfar[k] = FLT_MAX;
				next_ray_packet.ray_packet->geomID[k] = RTC_INVALID_GEOMETRY_ID;
				next_ray_packet.ray_packet->primID[k] = RTC_INVALID_GEOMETRY_ID;
				next_ray_packet.ray_packet->mask[k] = -1;
				next_ray_packet.ray_packet->time[k] = 0;

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

bool Camera::GetRays(CompleteRay first_ray, int tile_index, int *indices)
{
	if(tile_index>= ((film->width/ CAMERA_TILE_WIDTH)* (film->height/ CAMERA_TILE_HEIGHT)))
		return false;

#if ISPC_GET_RAYS
	//This has not been tested

	/*float position_[3]; SetFloat3(position_, position);
	float forward_[3]; SetFloat3(forward_, forward);
	float view_plane_u_[3]; SetFloat3(view_plane_u_, view_plane_u);
	float view_plane_v_[3]; SetFloat3(view_plane_v_, view_plane_v);*/

	int tile_count_x= film->width/ CAMERA_TILE_WIDTH;
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;
	int x_offset= tile_x* CAMERA_TILE_WIDTH;
	int y_offset= tile_y* CAMERA_TILE_HEIGHT;

	float *pos= new float[4]; SetFloat3(pos, position);
	float *forw= new float[4]; SetFloat3(forw, forward);
	float *u= new float[4]; SetFloat3(u, view_plane_u);
	float *v= new float[4]; SetFloat3(v, view_plane_v);

	/*ispc::GetRayDirections(x_offset, y_offset,
							pos, forw, 
							u, v,
							film->width, film->height, 
							reinterpret_cast<ispc::Ray *>(first_ray.ray));
							*/
	delete pos, forw, u, v;

	
	CompleteRay next_ray= first_ray;
	for(int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		for(int i= 0; i< CAMERA_TILE_WIDTH; i++)
		{
			int x= i+ tile_x* CAMERA_TILE_WIDTH;
			int y= j+ tile_y* CAMERA_TILE_HEIGHT;
			if(x< film->width && y< film->height);
			else
				continue;

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

float * Camera::GetFilteringKernels()
{
	return filtering_kernels;
}