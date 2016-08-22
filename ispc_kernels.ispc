#include "Parameters.h"

#define RTC_INVALID_GEOMETRY_ID ((unsigned int)-1)

//Want some way to 1) make my classes standard across ISPC and C++, and
//2) Not have to update my RTC structures when embree changes
//CAn accomplish 2 by figuring out how to include embree's ISPC interface
//CAn accomplish 1 by having mangled name base class in C format and then derive
//In C++ with correct name, typedef it here.
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
							uniform RayPacketExtras ray_packet_extrass[])
{
	for(uniform int i= 0; i< RAY_PACKET_BLOCK_SIZE; i++)
	{
		uniform int index= i* programCount;
		uniform int x_base= (index% CAMERA_TILE_WIDTH)+ x_offset;
		uniform int y= index/ CAMERA_TILE_WIDTH+ y_offset;

		int x= x_base+ programIndex;

		float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
		uniform float normalized_y= ((y+ 0.5f)/ height)* 2- 1;

		//print("x_base: %, y: %, x: %, normx: %\n", x_base, y, x, normalized_x);

		ray_packets[i].orgx[programIndex]= camera_position[0];
		ray_packets[i].orgy[programIndex]= camera_position[1];
		ray_packets[i].orgz[programIndex]= camera_position[2];

		//-normalized is quick hack
		ray_packets[i].dirx[programIndex]= camera_forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
		ray_packets[i].diry[programIndex]= camera_forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
		ray_packets[i].dirz[programIndex]= camera_forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

		ray_packets[i].tnear[programIndex] = 0.0f;
		ray_packets[i].tfar[programIndex] = 1000000000;
		ray_packets[i].geomID[programIndex] = RTC_INVALID_GEOMETRY_ID;
		ray_packets[i].primID[programIndex] = RTC_INVALID_GEOMETRY_ID;
		ray_packets[i].mask[programIndex] = -1;
		ray_packets[i].time[programIndex] = 0;

		ray_packet_extrass[i].absorption_r[programIndex]= 1.0f;
		ray_packet_extrass[i].absorption_g[programIndex]= 1.0f;
		ray_packet_extrass[i].absorption_b[programIndex]= 1.0f;

		ray_packet_extrass[i].bounce_count[programIndex]= 0;
		ray_packet_extrass[i].type[programIndex]= 0;
		ray_packet_extrass[i].x[programIndex]= x;
#if NO_OPENGL
		ray_packet_extrass[i].y[programIndex]= (height- 1)- y;
#else
		ray_packet_extrass[i].y[programIndex]= y;
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

#if SOA_RECEPTORS
export void Develop(uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], 
	                 uniform int8 image[], uniform float max_component, uniform int count)
{
	for(uniform int i= 0; i< count; i+= programCount)
	{
		int index= i+ programIndex;
		int image_offset= index* 4;

		image[image_offset+ 0]= (int8)(255* receptors_r[index]/ max_component);
		image[image_offset+ 1]= (int8)(255* receptors_g[index]/ max_component);
		image[image_offset+ 2]= (int8)(255* receptors_b[index]/ max_component);
	}
}

#else
export void Develop(uniform float receptors[], uniform int8 image[], uniform float max_component, uniform int count)
{
	foreach(index= 0 ... count)
	{
		image[index]= (int8)(255* receptors[index]/ max_component);
	}
}

#endif

export void PacketedShadingKernel(uniform RayPacket_ ray_packets[], 
							      uniform RayPacketExtras ray_packet_extrass[], 
#if SOA_RECEPTORS
								  uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], 
#else
								  uniform float receptors[],
#endif
								  uniform int film_width,
								  Lighting * uniform lighting)
{
	for(uniform int i= 0; i< RAY_PACKET_BLOCK_SIZE; i++)
	{
		cif(ray_packets[i].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID)
			continue;

		float<3> color= {0.0f, 0.0f, 0.0f};

		float<3> position= {ray_packets[i].orgx[programIndex]+ (ray_packets[i].dirx[programIndex]* ray_packets[i].tfar[programIndex]), 
			                ray_packets[i].orgy[programIndex]+ (ray_packets[i].diry[programIndex]* ray_packets[i].tfar[programIndex]), 
							ray_packets[i].orgz[programIndex]+ (ray_packets[i].dirz[programIndex]* ray_packets[i].tfar[programIndex])};

		float<3> normal= {-ray_packets[i].Ngx[programIndex], 
			              -ray_packets[i].Ngy[programIndex], 
						  -ray_packets[i].Ngz[programIndex]};
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

		color[0]*= ray_packet_extrass[i].absorption_r[programIndex];
		color[1]*= ray_packet_extrass[i].absorption_g[programIndex];
		color[2]*= ray_packet_extrass[i].absorption_b[programIndex];

		int x= ray_packet_extrass[i].x[programIndex];
		int y= ray_packet_extrass[i].y[programIndex];

		//Just realized this isn't thread safe here OR for our C++ versions if
		//We have multiple samples per pixel
#if SOA_RECEPTORS

		//This is fast but extremely specific- we are not going to have
		//One ray per pixel in the future so this will eventually need to be replaced
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

#else
		int receptor_offset= (y* film_width+ x+ programIndex)* 3;
		receptors[receptor_offset+ 0]= color[0];
		receptors[receptor_offset+ 1]= color[1];
		receptors[receptor_offset+ 2]= color[2];

#endif
	}
}

export void PacketedShadingKernel_Single(RayPacket_ * uniform ray_packet, 
							      RayPacketExtras * uniform ray_packet_extras, 
#if SOA_RECEPTORS
								  uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], 
#else
								  uniform float receptors[],
#endif
								  uniform int film_width,
								  Lighting * uniform lighting)
{
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

	//Just realized this isn't thread safe here OR for our C++ versions if
	//We have multiple samples per pixel
#if SOA_RECEPTORS

	//This is fast but extremely specific- we are not going to have
	//One ray per pixel in the future so this will eventually need to be replaced
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

#else
	int receptor_offset= (y* film_width+ x+ programIndex)* 3;
	receptors[receptor_offset+ 0]= color[0];
	receptors[receptor_offset+ 1]= color[1];
	receptors[receptor_offset+ 2]= color[2];

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