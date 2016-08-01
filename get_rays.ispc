#include "Parameters.h"

#define RTC_INVALID_GEOMETRY_ID ((unsigned int)-1)


struct Vec3f
{
	float x, y, z;
};

struct RTCRay
{
	//ray data
	float org[3];      //!< Ray origin
	float align0;
  
	float dir[3];      //!< Ray direction
	float align1;
  
	float tnear;       //!< Start of ray segment
	float tfar;        //!< End of ray segment (set to hit distance)

	float time;        //!< Time of this ray for motion blur
	unsigned int mask;        //!< Used to mask out objects during traversal
  
	//hit data
	float Ng[3];       //!< Unnormalized geometry normal
	float align2;
  
	float u;           //!< Barycentric u coordinate of hit
	float v;           //!< Barycentric v coordinate of hit

	unsigned int geomID;        //!< geometry ID
	unsigned int primID;        //!< primitive ID
	unsigned int instID;        //!< instance ID

	int8 padding[96- 84];//Not sure if padding is supposed to come before or after
};

struct RayAncillaries//Change name probably
{
	float x, y;
	float absorption[3];
	int bounce_count;
	int type;
};

//Should try making these references so no copying
export void GetRays(uniform int tile_index, 
					//varying Vec3f * uniform camera_position, varying Vec3f * uniform camera_forward,
					uniform float camera_position[3], uniform float camera_forward[3], 
					//uniform Vec3f view_plane_u, uniform Vec3f view_plane_v, 
					uniform float view_plane_u[3], uniform float view_plane_v[3],
					uniform int width, uniform int height, 
					uniform RTCRay rays[], uniform RayAncillaries ray_ancillariess[],
					uniform int count) 
{
	int tile_count_x= width/ TILE_WIDTH;
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

    foreach(index= 0 ... count) 
	{
        int x= index% TILE_WIDTH+ tile_x* TILE_WIDTH;
		int y= index/ TILE_WIDTH+ tile_y* TILE_HEIGHT;
		float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
		float normalized_y= ((y+ 0.5f)/ height)* 2- 1;

		rays[index].org[0]= camera_position[0];
		rays[index].org[1]= camera_position[1];
		rays[index].org[2]= camera_position[2];

		//-normalized is quick hack
		rays[index].dir[0]= camera_forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
		rays[index].dir[1]= camera_forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
		rays[index].dir[2]= camera_forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

		rays[index].tnear = 0.0f;
		rays[index].tfar = 1000000000.0f;
		rays[index].geomID = RTC_INVALID_GEOMETRY_ID;
		rays[index].primID = RTC_INVALID_GEOMETRY_ID;
		rays[index].mask = -1;
		rays[index].time = 0;

		ray_ancillariess[index].absorption[0]= 1.0f;
		ray_ancillariess[index].absorption[1]= 1.0f;
		ray_ancillariess[index].absorption[2]= 1.0f;

		ray_ancillariess[index].bounce_count= 0;
		ray_ancillariess[index].type= 0;
		ray_ancillariess[index].x= x;
		ray_ancillariess[index].y= y;
    }
}

export void Develop(uniform float receptors[], uniform int8 image[], uniform float max_component, uniform int count)
{
	foreach(index= 0 ... count)
	{
		image[index]= (int8)(255* receptors[index]/ max_component);
	}
}


/*
int tile_index= next_tile_index++;//will want to test this to see that it indeed works as expected
	int tile_count_x= film->width/ TILE_WIDTH;//May be best to compute this only once
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

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
			count++;

			
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

			next_ray.ray_ancillaries->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.ray_ancillaries->bounce_count= 0;
			next_ray.ray_ancillaries->type= RayType::Primary;
			next_ray.ray_ancillaries->x= x;
			next_ray.ray_ancillaries->y= y;

			next_ray.ray++;
			next_ray.ray_ancillaries++;
*/
