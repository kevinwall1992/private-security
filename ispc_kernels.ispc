#include "Parameters.h"

#define RTC_INVALID_GEOMETRY_ID ((unsigned int)-1)


struct Vec3f
{
	float x, y, z;
};

struct Ray
{
	float org[3];
	float align0;
  
	float dir[3];
	float align1;
  
	float tnear;
	float tfar;

	float time;
	unsigned int mask;
  

	float Ng[3];
	float align2;
  
	float u;
	float v;

	unsigned int geomID;
	unsigned int primID;
	unsigned int instID;


	int8 padding[96- 84];
};

struct RayExtras
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

export void GetRayDirections(uniform int x_offset, uniform int y_offset, 
							uniform float camera_position[], uniform float camera_forward[], 
							uniform float view_plane_u[], uniform float view_plane_v[],
							uniform int width, uniform int height,
							uniform Ray rays[])
{
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
}

export void Develop(uniform float receptors[], uniform int8 image[], uniform float max_component, uniform int count)
{
	foreach(index= 0 ... count)
	{
		image[index]= (int8)(255* receptors[index]/ max_component);
	}
}