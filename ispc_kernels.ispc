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

/*void soa_to_aos(soa<8> Vec3f pts_soa[], uniform int count,  uniform Vec3f pts_aos[]) 
{
     foreach (i = 0 ... count)
         pts_aos[i] = pts_soa[i];
}*/

//Should try making these references so no copying
export void GetRayDirections(uniform int x_offset, uniform int y_offset, 
							uniform float camera_position[], uniform float camera_forward[], 
							uniform float view_plane_u[], uniform float view_plane_v[],
							uniform int width, uniform int height,
							uniform RTCRay rays[])
{
#if 0
	uniform float xs[TILE_WIDTH];
	uniform float ys[TILE_HEIGHT];
	//uniform int ray_indices[TILE_WIDTH* TILE_HEIGHT];

	//print("Bar\n");

	foreach(x_index= 0 ... TILE_WIDTH)
	{
		xs[x_index]= -((((x_offset+ x_index)+ 0.5f)/ width)* 2- 1);
	}

	foreach(y_index= 0 ... TILE_HEIGHT)
	{
		ys[y_index]= (((y_offset+ y_index)+ 0.5f)/ height)* 2- 1;
	}

	/*for(uniform int j= 0; j< TILE_HEIGHT; j++)
	{
		for(uniform int i= 0; i< TILE_WIDTH; i++)
		{
			ray_indices[i+ j* TILE_WIDTH]= 
		}
	}*/

	//print("Foo\n");

	//print("xs: %\n\nys: %\n", xs, ys);

	for(uniform int ray_index= 0; ray_index< TILE_WIDTH* TILE_HEIGHT; ray_index++)
	{
		//print("i: %, xs_i: %, ys_i: %, x: %, y: %\n", ray_index, ray_index% TILE_WIDTH, ray_index/ TILE_WIDTH, xs[ray_index% TILE_WIDTH], ys[ray_index/ TILE_WIDTH]);

		rays[ray_index].org[programIndex]= camera_position[programIndex];
		rays[ray_index].dir[programIndex]= camera_forward[programIndex]+ view_plane_u[programIndex]* xs[ray_index% TILE_WIDTH]+ view_plane_v[programIndex]* ys[ray_index/ TILE_WIDTH];
	}

#else
	for(uniform int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		uniform float normalized_y= (((y_offset+ j)+ 0.5f)/ height)* 2- 1;

		for(uniform int i= 0; i< CAMERA_TILE_WIDTH; i++)
		{
			uniform float normalized_x= (((x_offset+ i)+ 0.5f)/ width)* 2- 1;
			uniform int ray_index= i+ j* CAMERA_TILE_WIDTH;

			rays[ray_index].org[programIndex]= camera_position[programIndex];
			rays[ray_index].dir[programIndex]= camera_forward[programIndex]+ view_plane_u[programIndex]* -normalized_x+ view_plane_v[programIndex]* normalized_y;
		}
	}

#endif
}

//Should try making these references so no copying
/*export void GetRays(uniform int x_offset, uniform int y_offset,
					//varying Vec3f * uniform camera_position, varying Vec3f * uniform camera_forward,
					uniform float camera_position[3], uniform float camera_forward[3], 
					//uniform Vec3f view_plane_u, uniform Vec3f view_plane_v, 
					uniform float view_plane_u[3], uniform float view_plane_v[3],
					uniform int width, uniform int height, 
					uniform RTCRay rays[], uniform RayAncillaries ray_ancillariess[],
					uniform int count) 
{
#if 1
	
	for(uniform int j= 0; j< TILE_HEIGHT; j++)//try cfor
	{
		uniform int y= j+ y_offset;
		uniform float normalized_y= ((y+ 0.5f)/ height)* 2- 1;
		uniform int foo= j* TILE_WIDTH;

		//This only works tile widths that are multiple of the programCount
		foreach(i= 0 ... TILE_WIDTH)
		{
			int x= i+ x_offset;
			float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
			int ray_index= i+ foo;



			rays[ray_index].org[0]= camera_position[0];
			rays[ray_index].org[1]= camera_position[1];
			rays[ray_index].org[2]= camera_position[2];

			

			rays[ray_index].tnear = 0.0f;
			rays[ray_index].tfar = 1000000000.0f;
			rays[ray_index].geomID = RTC_INVALID_GEOMETRY_ID;
			rays[ray_index].primID = RTC_INVALID_GEOMETRY_ID;
			rays[ray_index].mask = -1;
			rays[ray_index].time = 0;

			ray_ancillariess[ray_index].absorption[0]= 1.0f;
			ray_ancillariess[ray_index].absorption[1]= 1.0f;
			ray_ancillariess[ray_index].absorption[2]= 1.0f;

			ray_ancillariess[ray_index].bounce_count= 0;
			ray_ancillariess[ray_index].type= 0;
			ray_ancillariess[ray_index].x= x;
			ray_ancillariess[ray_index].y= y;

#if 1
			//-normalized is quick hack
			foreach_active(index)
			{
				uniform int ray_index_= extract(ray_index, index);
				uniform float normalized_x_= extract(normalized_x, index);
				uniform float normalized_y_= extract(normalized_y, index);

				float * uniform dir= &(rays[ray_index_].dir[0]);

				unmasked
				{
					if(programIndex< 3)
						dir[programIndex]= camera_forward[programIndex]+ view_plane_u[programIndex]* -normalized_x_+ view_plane_v[programIndex]* normalized_y_;
				}
			}
#else
			//-normalized is quick hack
			rays[ray_index].dir[0]= camera_forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
			rays[ray_index].dir[1]= camera_forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
			rays[ray_index].dir[2]= camera_forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;
#endif

		}
	}

#else
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
#endif
}*/

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
