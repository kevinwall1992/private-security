#include "Camera.h"


Camera::Camera(float fov_in_degrees, Vec3f position_, Vec3f direction_)
	: shutter(this)
{
	fov= 3.14f* (fov_in_degrees/ 180.0f);
	position= position_;
	forward= direction_;

	ComputeViewPlane();
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
	film->Clear();
	shutter.Open(scene);

	return &(film->image);
}

void Camera::GetRays(CompleteRay first_ray, int &count, int tile_index)
{
#if ISPC_GET_RAYS
	int tile_index= next_tile_index++;
	if(tile_index>= ((film->width/ TILE_WIDTH)* (film->height/ TILE_HEIGHT)))
	{
		count= 0;
		return;
	}

	/*float position_[3]; SetFloat3(position_, position);
	float forward_[3]; SetFloat3(forward_, forward);
	float view_plane_u_[3]; SetFloat3(view_plane_u_, view_plane_u);
	float view_plane_v_[3]; SetFloat3(view_plane_v_, view_plane_v);*/

	int tile_count_x= film->width/ TILE_WIDTH;
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;
	int x_offset= tile_x* TILE_WIDTH;
	int y_offset= tile_y* TILE_HEIGHT;

	float *pos= new float[4]; SetFloat3(pos, position);
	float *forw= new float[4]; SetFloat3(forw, forward);
	float *u= new float[4]; SetFloat3(u, view_plane_u);
	float *v= new float[4]; SetFloat3(v, view_plane_v);

	ispc::GetRayDirections(x_offset, y_offset,
							pos, forw, 
							u, v,
							film->width, film->height, 
							reinterpret_cast<ispc::Ray *>(first_ray.ray));

	delete pos, forw, u, v;

	
	CompleteRay next_ray= first_ray;
	for(int j= 0; j< TILE_HEIGHT; j++)
	{
		for(int i= 0; i< TILE_WIDTH; i++)
		{
			int x= i+ tile_x* TILE_WIDTH;
			int y= j+ tile_y* TILE_HEIGHT;
			if(x< film->width && y< film->height);
			else
				continue;

			next_ray.ray->tnear = 0.0f;
			next_ray.ray->tfar = FLT_MAX;
			next_ray.ray->geomID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->primID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->mask = -1;
			next_ray.ray->time = 0;

			next_ray.ray_extras->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.ray_extras->bounce_count= 0;
			next_ray.ray_extras->type= RayType::Primary;
			next_ray.ray_extras->x= x;
			next_ray.ray_extras->y= y;

			next_ray.ray++;
			next_ray.ray_extras++;
		}
	}

#else
	if(tile_index>= ((film->width/ CAMERA_TILE_WIDTH)* (film->height/ CAMERA_TILE_HEIGHT)))
	{
		count= 0;
		return;
	}

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

#endif
}