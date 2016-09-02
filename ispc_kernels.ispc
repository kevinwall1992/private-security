struct Vec2f
{
	float x, y;
};

struct Vec3f
{
	float x, y, z;
};

#define ISPC_CODE
#include "Parameters.h"
#include "Sampling.h"
#undef ISPC_CODE

#define RTC_INVALID_GEOMETRY_ID ((unsigned int)-1)

//Want some way to 1) make my classes standard across ISPC and C++, and
//2) Not have to update my RTC structures when embree changes
//Can accomplish 2 by figuring out how to include embree's ISPC interface
//Can accomplish 1 by having mangled name base class in C format and then derive
//In C++ with correct name, typedef it here.

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

struct RayPacket_
{
	float orgx[PACKET_SIZE];
	float orgy[PACKET_SIZE];
	float orgz[PACKET_SIZE]; 
  
	float dirx[PACKET_SIZE]; 
	float diry[PACKET_SIZE]; 
	float dirz[PACKET_SIZE]; 
  
	float tnear[PACKET_SIZE]; 
	float tfar[PACKET_SIZE];

	float time[PACKET_SIZE]; 
	unsigned int mask[PACKET_SIZE]; 
  

	float Ngx[PACKET_SIZE]; 
	float Ngy[PACKET_SIZE];
	float Ngz[PACKET_SIZE];
  
	float u[PACKET_SIZE];
	float v[PACKET_SIZE];
  
	unsigned int geomID[PACKET_SIZE]; 
	unsigned int primID[PACKET_SIZE]; 
	unsigned int instID[PACKET_SIZE]; 
};

//what would be the benefit of aligning this?
struct RayPacketExtras
{
	float x[PACKET_SIZE], y[PACKET_SIZE];
	float absorption_r[PACKET_SIZE], absorption_g[PACKET_SIZE], absorption_b[PACKET_SIZE];
	int bounce_count[PACKET_SIZE];
	int type[PACKET_SIZE];

	float surface_normal_x[PACKET_SIZE];
	float surface_normal_y[PACKET_SIZE];
	float surface_normal_z[PACKET_SIZE];
};

//check that short vectors are as good as arrays
struct PointLight_
{
	float position[3];
	float intensity[3];
};

struct AmbientLight
{
	float intensity[3];
};

struct Lighting
{
	PointLight_ *point_lights;
	float ambient[3];

	int point_light_count;
};

//Try making point a pointer (heh)
//need to make sure lighting is a uniform, not sure how yet
//Think this should take surface normal as well
/*float<3> GetLuminosityAtPoint(float<3> point, Lighting *lighting, uniform int light_index)
{
	uniform int index= light_index;
	if(index< lighting->point_light_count)
	{
		return lighting->point_lights[index].intensity;
	}

	index= index- lighting->point_light_count;
	if(index< lighting->ambient_light_count)
	{
		return lighting->ambient_lights[index].intensity;
	}

	float<3> luminosity= {0.0f, 0.0f, 0.0f};
	return luminosity;
}*/

/*void soa_to_aos(soa<8> Vec3f pts_soa[], uniform int count,  uniform Vec3f pts_aos[]) 
{
     foreach (i = 0 ... count)
         pts_aos[i] = pts_soa[i];
}*/

export void GetRayPackets(uniform int x_offset, uniform int y_offset, 
							uniform float camera_position[], uniform float camera_forward[], 
							uniform float view_plane_u[], uniform float view_plane_v[],
							uniform int width, uniform int height,
							uniform RayPacket_ ray_packets[], 
							uniform RayPacketExtras ray_packet_extrass[], 
							uniform float samples_x[], uniform float samples_y[],
							uniform int count,
							uniform int indices[])
{
	for(uniform int i= 0; i< count; i++)
	{
		uniform int x_base;
		uniform int y;
		if(indices== NULL)
		{
			uniform int index= i* (programCount/ MIN_SAMPLES_PER_PIXEL);
			x_base= (index% CAMERA_TILE_WIDTH)+ x_offset;
			y= index/ CAMERA_TILE_WIDTH+ y_offset;
		}
		else
		{
			uniform int index= indices[i];

			x_base= index% width;
			y= (height- 1)- (index/ height);//map to view plane space
		}
		uniform int packet_index= i;

		//These are hardcoded for 256 bit vectorization 
#if MIN_SAMPLES_PER_PIXEL== 1
		int x= x_base+ programIndex;
		float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
		uniform float normalized_y= ((y+ 0.5f)/ height)* 2- 1;

#elif MIN_SAMPLES_PER_PIXEL== 4
		int set_index= programIndex/ MIN_SAMPLES_PER_PIXEL;
		int x= x_base+ set_index;
		float normalized_x= ((x+ HaltonSequence(2, programIndex- set_index* MIN_SAMPLES_PER_PIXEL))/ width)* 2- 1;
		float normalized_y= ((y+ HaltonSequence(3, programIndex- set_index* MIN_SAMPLES_PER_PIXEL))/ height)* 2- 1;

#elif MIN_SAMPLES_PER_PIXEL== 8
		uniform int x= x_base;
		float normalized_x= ((x+ samples_x[programIndex])/ width)* 2- 1;
		float normalized_y= ((y+ samples_y[programIndex])/ height)* 2- 1;

#endif

		ray_packets[packet_index].orgx[programIndex]= camera_position[0];
		ray_packets[packet_index].orgy[programIndex]= camera_position[1];
		ray_packets[packet_index].orgz[programIndex]= camera_position[2];

		//-normalized is quick hack
		ray_packets[packet_index].dirx[programIndex]= camera_forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
		ray_packets[packet_index].diry[programIndex]= camera_forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
		ray_packets[packet_index].dirz[programIndex]= camera_forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

		ray_packets[packet_index].tnear[programIndex] = 0.0f;
		ray_packets[packet_index].tfar[programIndex] = 1000000000;
		ray_packets[packet_index].geomID[programIndex] = RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].primID[programIndex] = RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].mask[programIndex] = -1;
		ray_packets[packet_index].time[programIndex] = 0;

		ray_packet_extrass[packet_index].absorption_r[programIndex]= 1.0f;
		ray_packet_extrass[packet_index].absorption_g[programIndex]= 1.0f;
		ray_packet_extrass[packet_index].absorption_b[programIndex]= 1.0f;

		ray_packet_extrass[packet_index].bounce_count[programIndex]= 0;
		ray_packet_extrass[packet_index].type[programIndex]= 0;
		ray_packet_extrass[packet_index].x[programIndex]= x;
#if NO_OPENGL
		ray_packet_extrass[packet_index].y[programIndex]= (height- 1)- y;//map to image space
#else
		ray_packet_extrass[packet_index].y[programIndex]= y;
#endif

	}
}

export void GetRayDirections(uniform int x_offset, uniform int y_offset, 
							uniform float camera_position[], uniform float camera_forward[], 
							uniform float view_plane_u[], uniform float view_plane_v[],
							uniform int width, uniform int height,
							uniform Ray rays[])
{
//Just to quiet warnings
#if !PACKET_MODE
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

export void Develop(uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], uniform int sample_counts[], 
	                 uniform int8 image[], uniform float normalization_term, uniform int count)
{
	for(uniform int i= 0; i< count; i+= programCount)
	{
		int index= i+ programIndex;
		int image_offset= index* 4;

#if FILTERING
		image[image_offset+ 0]= max((int)(255* receptors_r[index]/ (normalization_term* sample_counts[index])), (int)0);
		image[image_offset+ 1]= max((int)(255* receptors_g[index]/ (normalization_term* sample_counts[index])), (int)0);
		image[image_offset+ 2]= max((int)(255* receptors_b[index]/ (normalization_term* sample_counts[index])), (int)0);
#else
		image[image_offset+ 0]= (int8)(255* receptors_r[index]/ (normalization_term* sample_counts[index]));
		image[image_offset+ 1]= (int8)(255* receptors_g[index]/ (normalization_term* sample_counts[index]));
		image[image_offset+ 2]= (int8)(255* receptors_b[index]/ (normalization_term* sample_counts[index]));
#endif

#if 0
		if(sample_counts[index]> 39)
		{
			image[image_offset+ 0]= 0;
			image[image_offset+ 1]= 255;
			image[image_offset+ 2]= 0;
		}
		else
		{
			image[image_offset+ 0]= 0;
			image[image_offset+ 1]= 0;
			image[image_offset+ 2]= 255;
		}

		if(receptors_r[index]< 0.000001f)
		{
			image[image_offset+ 0]= 255;
			image[image_offset+ 1]= 0;
			image[image_offset+ 2]= 0;
		}
#endif

	}
}

export void PacketedShadingKernel(uniform RayPacket_ ray_packets[], 
							      uniform RayPacketExtras ray_packet_extrass[], 
								  uniform int packet_count,
								  uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], uniform int sample_counts[],
								  uniform int film_width,
								  Lighting * uniform lighting, 
								  uniform float filtering_kernels[],
								  uniform int noisy_receptors[], uniform int noisy_receptor_count[])
{
	//if(high_variance_pixel_indices!= NULL)
	//	high_variance_pixel_count[0]= 0;

	for(uniform int i= 0; i< packet_count; i++)
	{
		cif(ray_packets[i].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID)
			continue;

		float<3> color= {0.0f, 0.0f, 0.0f};

		float<3> position= {ray_packets[i].orgx[programIndex]+ (ray_packets[i].dirx[programIndex]* ray_packets[i].tfar[programIndex]), 
			                ray_packets[i].orgy[programIndex]+ (ray_packets[i].diry[programIndex]* ray_packets[i].tfar[programIndex]), 
							ray_packets[i].orgz[programIndex]+ (ray_packets[i].dirz[programIndex]* ray_packets[i].tfar[programIndex])};

		float<3> normal= {ray_packet_extrass[i].surface_normal_x[programIndex], 
			              ray_packet_extrass[i].surface_normal_y[programIndex], 
						  ray_packet_extrass[i].surface_normal_z[programIndex]};

		for(uniform int j= 0; j< lighting->point_light_count; j++)
		{
			float<3> direction;
			direction[0]= lighting->point_lights[j].position[0]- position[0];
			direction[1]= lighting->point_lights[j].position[1]- position[1];
			direction[2]= lighting->point_lights[j].position[2]- position[2];
			float direction_magnitude= sqrt(direction[0]* direction[0]+ 
				                            direction[1]* direction[1]+ 
											direction[2]* direction[2]);

			float geometry_term= (direction[0]* normal[0]+ 
				                  direction[1]* normal[1]+ 
								  direction[2]* normal[2])/ direction_magnitude;

			if(geometry_term> 0)
			{
				color[0]+= lighting->point_lights[j].intensity[0]* geometry_term;
				color[1]+= lighting->point_lights[j].intensity[1]* geometry_term;
				color[2]+= lighting->point_lights[j].intensity[2]* geometry_term;
			}
		}
		color[0]+= lighting->ambient[0];
		color[1]+= lighting->ambient[1];
		color[2]+= lighting->ambient[2];

		color[0]*= ray_packet_extrass[i].absorption_r[programIndex];
		color[1]*= ray_packet_extrass[i].absorption_g[programIndex];
		color[2]*= ray_packet_extrass[i].absorption_b[programIndex];


		int x= ray_packet_extrass[i].x[programIndex];
		int y= ray_packet_extrass[i].y[programIndex];

#if FILTERING
		uniform int uniform_y;
		uniform int uniform_x;
		if(reduce_equal(x, &uniform_x) && reduce_equal(y, &uniform_y))
		{
			for(uniform int j= 0; j< 9; j++)
			{
				float weight= filtering_kernels[j* 8+ programIndex];

				uniform float<3> color_sum= {reduce_add(color[0]* weight), reduce_add(color[1]* weight), reduce_add(color[2]* weight)};

				uniform int receptor_index= (uniform_y+ ((j/ 3)- 1))* film_width+ (uniform_x+ ((j% 3)- 1));
				receptors_r[receptor_index]+= color_sum[0];
				receptors_g[receptor_index]+= color_sum[1];
				receptors_b[receptor_index]+= color_sum[2];
			}

			if(high_variance_pixel_indices!= NULL)
				sample_counts[uniform_y* film_width+ uniform_x]= MIN_SAMPLES_PER_PIXEL;
		}
		else
		{
			int receptor_index= y* film_width+ x;
			receptors_r[receptor_index]= 1.0f;
			receptors_g[receptor_index]= 0.0f;
			receptors_b[receptor_index]= 0.0f;
		}

#else
#if MIN_SAMPLES_PER_PIXEL== 1
		uniform int uniform_y;
		uniform int packet_x;
		if(reduce_equal(y, &uniform_y) && reduce_equal(x/ programCount, &packet_x))
		{
			int receptor_index= uniform_y* film_width+ packet_x* programCount+ programIndex;
			receptors_r[receptor_index]= color[0];
			receptors_g[receptor_index]= color[1];
			receptors_b[receptor_index]= color[2];

		}
		else
		{
			int receptor_index= y* film_width+ x;
			receptors_r[receptor_index]= 1.0f;
			receptors_g[receptor_index]= 0.0f;
			receptors_b[receptor_index]= 0.0f;
		}

#else
		uniform int uniform_y;
		uniform int uniform_x;
		if(reduce_equal(x, &uniform_x) && reduce_equal(y, &uniform_y))
		{
			uniform float<3> color_sum= {reduce_add(color[0]), reduce_add(color[1]), reduce_add(color[2])};

			uniform int receptor_index= uniform_y* film_width+ uniform_x;
			receptors_r[receptor_index]+= color_sum[0];
			receptors_g[receptor_index]+= color_sum[1];
			receptors_b[receptor_index]+= color_sum[2];

#if ADAPTIVE_SAMPLING
			if(noisy_receptors!= NULL)
			{
				uniform float<3> color_mean= color_sum/ programCount;
				float<3> displacement= (color- color_mean);//try using intrisic to square resulting vector
				float squared_distance= displacement[0]* displacement[0]+ displacement[1]* displacement[1]+ displacement[2]* displacement[2];

				uniform float variance= reduce_add(squared_distance);
				if(variance> COLOR_VARIANCE_THRESHOLD)
				{
					uniform int foo= atomic_add_global(noisy_receptor_count, 1);
					noisy_receptors[foo]= receptor_index;

					sample_counts[receptor_index]= MAX_SAMPLES_PER_PIXEL;
				}
				else
					sample_counts[receptor_index]= MIN_SAMPLES_PER_PIXEL;

			}

#else
			sample_counts[receptor_index]= MIN_SAMPLES_PER_PIXEL;

#endif
		}

		//Could try foreach_unique for non-coherent (x,y) problem

#endif

#endif

	}
}

export void PacketedShadingKernel_Single(RayPacket_ * uniform ray_packet, 
							      RayPacketExtras * uniform ray_packet_extras, 
								  uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], 
								  uniform int film_width,
								  Lighting * uniform lighting)
{
#if MIN_SAMPLES_PER_PIXEL== 1
	cif(ray_packet->geomID[programIndex]== RTC_INVALID_GEOMETRY_ID)
		return;

	float<3> color= {0.0f, 0.0f, 0.0f};

	float<3> position= {ray_packet->orgx[programIndex]+ (ray_packet->dirx[programIndex]* ray_packet->tfar[programIndex]), 
			            ray_packet->orgy[programIndex]+ (ray_packet->diry[programIndex]* ray_packet->tfar[programIndex]), 
						ray_packet->orgz[programIndex]+ (ray_packet->dirz[programIndex]* ray_packet->tfar[programIndex])};

	float<3> normal= {-ray_packet->Ngx[programIndex], 
			            -ray_packet->Ngy[programIndex], 
						-ray_packet->Ngz[programIndex]};
	float normal_magnitude= sqrt(normal[0]* normal[0]+ 
		                            normal[1]* normal[1]+ 
									normal[2]* normal[2]);

	for(uniform int j= 0; j< lighting->point_light_count; j++)
	{
		float<3> direction;
		direction[0]= lighting->point_lights[j].position[0]- position[0];
		direction[1]= lighting->point_lights[j].position[1]- position[1];
		direction[2]= lighting->point_lights[j].position[2]- position[2];
		float direction_magnitude= sqrt(direction[0]* direction[0]+ 
				                        direction[1]* direction[1]+ 
										direction[2]* direction[2]);

		float geometry_term= (direction[0]* normal[0]+ 
				                direction[1]* normal[1]+ 
								direction[2]* normal[2])/ (normal_magnitude* direction_magnitude);

		if(geometry_term> 0)
		{
			color[0]+= lighting->point_lights[j].intensity[0]* geometry_term;
			color[1]+= lighting->point_lights[j].intensity[1]* geometry_term;
			color[2]+= lighting->point_lights[j].intensity[2]* geometry_term;
		}
	}
	color[0]+= lighting->ambient[0];
	color[1]+= lighting->ambient[1];
	color[2]+= lighting->ambient[2];

	color[0]*= ray_packet_extras->absorption_r[programIndex];
	color[1]*= ray_packet_extras->absorption_g[programIndex];
	color[2]*= ray_packet_extras->absorption_b[programIndex];

	int x= ray_packet_extras->x[programIndex];
	int y= ray_packet_extras->y[programIndex];


	uniform int uniform_y;
	uniform int packet_x;
	if(reduce_equal(y, &uniform_y) && reduce_equal(x/ programCount, &packet_x))
	{
		int receptor_index= uniform_y* film_width+ packet_x* programCount+ programIndex;
		receptors_r[receptor_index]= color[0];
		receptors_g[receptor_index]= color[1];
		receptors_b[receptor_index]= color[2];
	}
	else
	{
		int receptor_index= y* film_width+ x;
		receptors_r[receptor_index]= color[0];
		receptors_g[receptor_index]= color[1];
		receptors_b[receptor_index]= color[2];
	}
#endif
}

#if ISPC_CLEAR
export void Clear(uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], uniform int count)
{
	foreach(index= 0 ... count)
	{
		receptors_r[index]= 0.0f;
		receptors_g[index]= 0.0f;
		receptors_b[index]= 0.0f;
	}
}
#endif

//Assuming only one geometry for now
#if ISPC_INTERPOLATION
export void Interpolate(uniform RayPacket_ ray_packets[], uniform RayPacketExtras ray_packet_extrass[], 
	                   uniform int normal_indices[], uniform float normals[])
{
#if PACKET_MODE
	for(int i= 0; i< RAY_PACKET_BLOCK_SIZE; i++)
	{
		cif(ray_packets[i].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID)
			continue;

		float u= ray_packets[i].u[programIndex];
		float v= ray_packets[i].v[programIndex];
		float w= 1- u- v;

		//print("u: %,\nv: %,\nw: %\n\n", u, v, w);

		uniform int geometry_id;
		reduce_equal(ray_packets[i].geomID[programIndex], &geometry_id);

		int primitive_id= ray_packets[i].primID[programIndex];
		uniform int uniform_primitive_id;
		if(reduce_equal(primitive_id, &uniform_primitive_id))
		{
			uniform int normal0_index= normal_indices[uniform_primitive_id* 3+ 0];
			uniform int normal1_index= normal_indices[uniform_primitive_id* 3+ 1];
			uniform int normal2_index= normal_indices[uniform_primitive_id* 3+ 2];

			uniform float<3> normal0= {normals[normal0_index* 3+ 0], normals[normal0_index* 3+ 1], normals[normal0_index* 3+ 2]};
			uniform float<3> normal1= {normals[normal1_index* 3+ 0], normals[normal1_index* 3+ 1], normals[normal1_index* 3+ 2]};
			uniform float<3> normal2= {normals[normal2_index* 3+ 0], normals[normal2_index* 3+ 1], normals[normal2_index* 3+ 2]};

			ray_packet_extrass[i].surface_normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
			ray_packet_extrass[i].surface_normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
			ray_packet_extrass[i].surface_normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;
		}
		else
		{
			foreach_unique(unique_primitive_id in primitive_id)
			{
				int normal0_index= normal_indices[unique_primitive_id* 3+ 0];
				int normal1_index= normal_indices[unique_primitive_id* 3+ 1];
				int normal2_index= normal_indices[unique_primitive_id* 3+ 2];

				float<3> normal0= {normals[normal0_index* 3+ 0], normals[normal0_index* 3+ 1], normals[normal0_index* 3+ 2]};
				float<3> normal1= {normals[normal1_index* 3+ 0], normals[normal1_index* 3+ 1], normals[normal1_index* 3+ 2]};
				float<3> normal2= {normals[normal2_index* 3+ 0], normals[normal2_index* 3+ 1], normals[normal2_index* 3+ 2]};

				ray_packet_extrass[i].surface_normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
				ray_packet_extrass[i].surface_normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
				ray_packet_extrass[i].surface_normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;
			}
		}
	}
#else
#endif
}

#endif