struct Vec2f
{
	float x, y;
};

struct Vec3f
{
	float x, y, z;
};

#define ISPC_CODE
#include "EBRParameters.h"
#undef ISPC_CODE

#include <embree2\rtcore.isph>
#include <embree2\rtcore_ray.isph>


struct VisibilityRay
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

struct VisibilityRayPacket
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

struct Surface
{
	float position[3];
	float normal[3];
	intptr_t material;
};

struct Medium
{
	float refractive_index;
	int is_air;
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


	float x, y;
	float absorption[3];
	int bounce_count;
	int type;

	Surface surface_;
	Medium medium;

	//This probably screws up alignment, but I'm not testing 
	//this until it becomes relevant.
	float *light_coefficients;

	int8 padding[8];

};

struct SurfacePacket
{
	float position_x[PACKET_SIZE];
	float position_y[PACKET_SIZE];
	float position_z[PACKET_SIZE];

	float normal_x[PACKET_SIZE];
	float normal_y[PACKET_SIZE];
	float normal_z[PACKET_SIZE];

	int material_id[PACKET_SIZE];
};

struct MediumPacket
{
	float refractive_index[PACKET_SIZE];
	int is_air[PACKET_SIZE];
};

struct RayPacket
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


	float x[PACKET_SIZE], y[PACKET_SIZE];
	float absorption_r[PACKET_SIZE], absorption_g[PACKET_SIZE], absorption_b[PACKET_SIZE];
	int bounce_count[PACKET_SIZE];
	int type[PACKET_SIZE];

	//Can't use "surface" for some reason
	SurfacePacket surface_;
	MediumPacket medium;

	float *light_coefficients;
	int8 padding[24];
};

//check that short vectors are as good as arrays
struct PointLight
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
	PointLight *point_lights;
	float ambient[3];

	int point_light_count;
};

struct Mesh
{
	float *positions;
	float *texture_coordinates;
	float *normals;

	int *position_indices;
	int *texture_coordinate_indices;
	int *normal_indices;

	int vertex_count;
	int triangle_count;

	int8 indirect_only;
};

struct PhongMaterial
{
	float diffuse[3];
	float specular[3];
	float glossiness;

	float reflectivity;
	float refractive_index;
	float transparency;
};

struct Data
{
	struct
	{
		float *normals;
		float *directions;
		float *image_xys;
		int *bounce_counts;
		int *incoming_ray_types;
		int *outgoing_ray_types;

		int *indicies;

		int count;
		int max_count;
		int collect_data;
	} shading;
};

void LogData_Shading_Reflection(uniform Data * uniform data, 
								varying float normal_x, varying float normal_y, varying float normal_z, 
								varying float direction_x, varying float direction_y, varying float direction_z, 
								varying float image_x, varying float image_y, 
								varying int bounce_count, 
								varying int incoming_ray_type,
								varying int outgoing_ray_type)
{
#if DATA_COLLECTION
	if(!data->shading.collect_data)
		return;

	uniform int lane_mask= lanemask();
	int index;
	uniform int active_count;
	unmasked
	{
		index= data->shading.count+ exclusive_scan_add(lane_mask& (1 << programIndex) ? 1 : 0);
	}
	if(index>= data->shading.max_count)
		return;
	lane_mask= lanemask();
	unmasked
	{
		data->shading.count+= reduce_add(lane_mask& (1 << programIndex) ? 1 : 0);
	}
	
	data->shading.normals[index* 3+ 0]= normal_x;
	data->shading.normals[index* 3+ 1]= normal_y;
	data->shading.normals[index* 3+ 2]= normal_z;

	data->shading.directions[index* 3+ 0]= direction_x;
	data->shading.directions[index* 3+ 1]= direction_y;
	data->shading.directions[index* 3+ 2]= direction_z;

	data->shading.image_xys[index* 2+ 0]= image_x;
	data->shading.image_xys[index* 2+ 1]= image_y;

	data->shading.bounce_counts[index]= bounce_count;
	data->shading.incoming_ray_types[index]= incoming_ray_type;
	data->shading.outgoing_ray_types[index]= outgoing_ray_type;

	data->shading.indicies[index]= index;

	data->shading.count+= active_count;
#endif
}

#define ISPC_CODE
#include "ISPCInterop.h"
#include "Sampling.h"
#undef ISPC_CODE


export void GetRayPackets(uniform int x_offset, uniform int y_offset, 
							uniform float camera_position[], uniform float camera_forward[], 
							uniform float view_plane_u[], uniform float view_plane_v[],
							uniform int width, uniform int height,
							uniform RayPacket ray_packets[], 
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

#if MIN_SAMPLES_PER_PIXEL== 1
		int x= x_base+ programIndex;
		float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
		uniform float normalized_y= ((y+ 0.5f)/ height)* 2- 1;

#elif MIN_SAMPLES_PER_PIXEL== 8
		uniform int x= x_base;
		float normalized_x= ((x+ samples_x[programIndex])/ width)* 2- 1;
		float normalized_y= ((y+ samples_y[programIndex])/ height)* 2- 1;

#endif

		ray_packets[packet_index].orgx[programIndex]= camera_position[0];
		ray_packets[packet_index].orgy[programIndex]= camera_position[1];
		ray_packets[packet_index].orgz[programIndex]= camera_position[2];

		ray_packets[packet_index].dirx[programIndex]= camera_forward[0]+ view_plane_u[0]* normalized_x+ view_plane_v[0]* normalized_y;
		ray_packets[packet_index].diry[programIndex]= camera_forward[1]+ view_plane_u[1]* normalized_x+ view_plane_v[1]* normalized_y;
		ray_packets[packet_index].dirz[programIndex]= camera_forward[2]+ view_plane_u[2]* normalized_x+ view_plane_v[2]* normalized_y;

		ray_packets[packet_index].tnear[programIndex]= 0.0f;
		ray_packets[packet_index].tfar[programIndex]= 1000000000;
		ray_packets[packet_index].geomID[programIndex]= RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].primID[programIndex]= RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].mask[programIndex]= -1;
		ray_packets[packet_index].time[programIndex]= 0;

		ray_packets[packet_index].absorption_r[programIndex]= 1.0f;
		ray_packets[packet_index].absorption_g[programIndex]= 1.0f;
		ray_packets[packet_index].absorption_b[programIndex]= 1.0f;

		ray_packets[packet_index].bounce_count[programIndex]= 0;
		ray_packets[packet_index].type[programIndex]= 0;
		ray_packets[packet_index].x[programIndex]= x;
		ray_packets[packet_index].y[programIndex]= y;

		ray_packets[packet_index].medium.refractive_index[programIndex]= 1.0f;
		ray_packets[packet_index].medium.is_air[programIndex]= true;
	}
}

export void GetRayPackets_Compositing(uniform int x_offset, uniform int y_offset, 
									uniform float camera_position[], uniform float camera_forward[], 
									uniform float view_plane_u[], uniform float view_plane_v[],
									uniform int width, uniform int height,
									uniform RayPacket ray_packets[], 
									uniform int count)
{
	for(uniform int i= 0; i< count; i++)
	{
		uniform int index= i* programCount;
		uniform int packet_index= i;

		int x= ((index+ programIndex)% 64)+ x_offset;
		uniform int y= index/ 64+ y_offset;
		float normalized_x= ((x+ 0.5f)/ width)* 2- 1;
		uniform float normalized_y= ((y+ 0.5f)/ height)* 2- 1;


		ray_packets[packet_index].orgx[programIndex]= camera_position[0];
		ray_packets[packet_index].orgy[programIndex]= camera_position[1];
		ray_packets[packet_index].orgz[programIndex]= camera_position[2];

		ray_packets[packet_index].dirx[programIndex]= camera_forward[0]+ view_plane_u[0]* normalized_x+ view_plane_v[0]* normalized_y;
		ray_packets[packet_index].diry[programIndex]= camera_forward[1]+ view_plane_u[1]* normalized_x+ view_plane_v[1]* normalized_y;
		ray_packets[packet_index].dirz[programIndex]= camera_forward[2]+ view_plane_u[2]* normalized_x+ view_plane_v[2]* normalized_y;

		ray_packets[packet_index].tnear[programIndex]= 0.0f;
		ray_packets[packet_index].tfar[programIndex]= 1000000000;
		ray_packets[packet_index].geomID[programIndex]= RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].primID[programIndex]= RTC_INVALID_GEOMETRY_ID;
		ray_packets[packet_index].mask[programIndex]= -1;
		ray_packets[packet_index].time[programIndex]= 0;

		ray_packets[packet_index].x[programIndex]= x;
		ray_packets[packet_index].y[programIndex]= y;
	}
}

export void Develop(uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], uniform int sample_counts[], 
	                 uniform int8 image[], uniform float normalization_term, uniform int count)
{
	for(uniform int i= 0; i< count; i+= programCount)
	{
		int index= i+ programIndex;
		int image_offset= index* 4;

#if FILTERING
#if ADAPTIVE_SAMPLING
		image[image_offset+ 0]= max((int)(255* receptors_r[index]/ (normalization_term* sample_counts[index])), (int)0);
		image[image_offset+ 1]= max((int)(255* receptors_g[index]/ (normalization_term* sample_counts[index])), (int)0);
		image[image_offset+ 2]= max((int)(255* receptors_b[index]/ (normalization_term* sample_counts[index])), (int)0);
#else
		image[image_offset+ 0]= max((int)(255* receptors_r[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL)), (int)0);
		image[image_offset+ 1]= max((int)(255* receptors_g[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL)), (int)0);
		image[image_offset+ 2]= max((int)(255* receptors_b[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL)), (int)0);
#endif
#else
#if ADAPTIVE_SAMPLING
		image[image_offset+ 0]= (int8)(255* receptors_r[index]/ (normalization_term* sample_counts[index]));
		image[image_offset+ 1]= (int8)(255* receptors_g[index]/ (normalization_term* sample_counts[index]));
		image[image_offset+ 2]= (int8)(255* receptors_b[index]/ (normalization_term* sample_counts[index]));
#else
		image[image_offset+ 0]= (int8)(255* receptors_b[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL));
		image[image_offset+ 1]= (int8)(255* receptors_g[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL));
		image[image_offset+ 2]= (int8)(255* receptors_r[index]/ (normalization_term* MIN_SAMPLES_PER_PIXEL));
#endif

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

void WriteRayPacket_Coalesced(uniform RayPacket * uniform ray_packet, 
							uniform int ray_offset,
							varying float<3> position, varying float<3> direction, 
							uniform float tnear, uniform float tfar, 
							varying float x, varying float y,
							varying int bounce_count, varying int type, 
							varying float refractive_index, varying bool in_air,
							varying float absorption_r, varying float absorption_g, varying float absorption_b)
{
	int ray_index= ray_offset+ programIndex;

	(ray_packet->orgx+ ray_offset)[programIndex]= position[0];
	(ray_packet->orgy+ ray_offset)[programIndex]= position[1];
	(ray_packet->orgz+ ray_offset)[programIndex]= position[2];

	(ray_packet->dirx+ ray_offset)[programIndex]= direction[0];
	(ray_packet->diry+ ray_offset)[programIndex]= direction[1];
	(ray_packet->dirz+ ray_offset)[programIndex]= direction[2];

	(ray_packet->tnear+ ray_offset)[programIndex]= tnear;
	(ray_packet->tfar+ ray_offset)[programIndex]= tfar;
	(ray_packet->geomID+ ray_offset)[programIndex]= RTC_INVALID_GEOMETRY_ID;
	(ray_packet->primID+ ray_offset)[programIndex]= RTC_INVALID_GEOMETRY_ID;
	(ray_packet->time+ ray_offset)[programIndex]= 0;

	(ray_packet->bounce_count+ ray_offset)[programIndex]= bounce_count;
	(ray_packet->type+ ray_offset)[programIndex]= type;
	(ray_packet->x+ ray_offset)[programIndex]= x;
	(ray_packet->y+ ray_offset)[programIndex]= y;

	(ray_packet->absorption_r+ ray_offset)[programIndex]= absorption_r;
	(ray_packet->absorption_g+ ray_offset)[programIndex]= absorption_g;
	(ray_packet->absorption_b+ ray_offset)[programIndex]= absorption_b;

	(ray_packet->medium.refractive_index+ ray_offset)[programIndex]= refractive_index;
	(ray_packet->medium.is_air+ ray_offset)[programIndex]= in_air;

	(ray_packet->mask+ ray_offset)[programIndex]= -1;
}

//Get rid of need for mask
unmasked void WriteRayPacket_Coherent(RayPacket * uniform ray_packets, 
									uniform int * uniform packet_count, uniform int * uniform front_packet_ray_count,
									varying int mask, 
									varying float<3> position, varying float<3> direction, 
									uniform float tnear, uniform float tfar, 
									varying float x, varying float y,
									varying int bounce_count, varying int type, 
									varying float refractive_index, varying bool in_air,
									varying float absorption_r, varying float absorption_g, varying float absorption_b)
{
	int series_id= exclusive_scan_add(mask ? 0 : 1);
	if(!mask)
		return;

	uniform int ray_count= reduce_add(1);
	
	foreach_unique(series in series_id)
	{
		uniform int series_ray_count= reduce_add(1);

		uniform int min_program_index= (series_ray_count== PACKET_SIZE) ? 0 : reduce_min(programIndex);
		
#if 1
		if((series_ray_count+ front_packet_ray_count[0])> PACKET_SIZE)
		{
			if((programIndex- min_program_index+ front_packet_ray_count[0])< PACKET_SIZE)
			{
				//print("b%\n", series_id);

				WriteRayPacket_Coalesced(ray_packets+ packet_count[0], front_packet_ray_count[0]- min_program_index,
										position, direction,
										tnear, tfar, 
										x, y,
										bounce_count, type, 
										refractive_index, in_air,
										absorption_r, absorption_g, absorption_b);
			}
			else
			{
				WriteRayPacket_Coalesced(ray_packets+ (packet_count[0]+ 1), -reduce_min(programIndex),
										position, direction, 
										tnear, tfar, 
										x, y,
										bounce_count, type, 
										refractive_index, in_air,
										absorption_r, absorption_g, absorption_b);
			}

			packet_count[0]++;
			front_packet_ray_count[0]= series_ray_count- (PACKET_SIZE- front_packet_ray_count[0]);
		}
		else
		{
			WriteRayPacket_Coalesced(ray_packets+ packet_count[0], front_packet_ray_count[0]- min_program_index,
									position, direction, 
									tnear, tfar, 
									x, y,
									bounce_count, type, 
									refractive_index, in_air,
									absorption_r, absorption_g, absorption_b);

			front_packet_ray_count[0]+= series_ray_count;
		}

#else
		if((series_ray_count+ front_packet_ray_count[0])> PACKET_SIZE)
		{
			unmasked { ray_packets[packet_count[0]++].mask[programIndex]= (programIndex< front_packet_ray_count[0]) ? -1 : 0; }
			front_packet_ray_count[0]= 0;
		}

		WriteRayPacket_Coalesced(ray_packets+ packet_count[0], front_packet_ray_count[0]- reduce_min(programIndex),
								position, direction, 
								tnear, tfar, 
								x, y,
								bounce_count, type, 
								refractive_index, in_air,
								absorption_r, absorption_g, absorption_b);

		front_packet_ray_count[0]+= series_ray_count;

#endif
	}

}

//Need to start cutting down on this...
export void PacketedShadingKernel_Coherent(uniform RayPacket ray_packets[], 
										  uniform int packet_count, 
										  uniform int ** uniform order, uniform int max_packet_index,
										  uniform int * uniform packets_shaded_count, 
										  uniform float receptors_r[], uniform float receptors_g[], uniform float receptors_b[], uniform int sample_counts[],
										  uniform int film_width,
										  Lighting * uniform lighting,
										  uniform PhongMaterial materials[],
										  uniform float filtering_kernels[],
										  uniform int noisy_receptors[], uniform int noisy_receptor_count[], 
#if PACKETED_SECONDARY_RAYS
										  uniform RayPacket secondary_ray_packets[],
										  uniform int * uniform secondary_packet_count,
#else
										  uniform Ray secondary_rays[], 
										  uniform int * uniform secondary_ray_count,
#endif
										  uniform int * uniform sample_index,
										  uniform Data * uniform data,
										  uniform float primary_disc_samples[],
										  uniform int disc_sample_indices[],
										  uniform float * uniform interval_samples,
										  uniform int * uniform interval_sample_indices,
										  uniform int frame_count)
{
	uniform int secondary_ray_count= 0;

#if RANDOM_PACKET_ORDER
	for(;packets_shaded_count[0]< packet_count; order[0]++)
#else
	for(uniform int i= 0; i< packet_count; i++)
#endif
	{
#if RANDOM_PACKET_ORDER
		uniform int i= order[0][0];
		if(i> max_packet_index)
			continue;
#endif

#if PACKETED_SECONDARY_RAYS
		if(secondary_packet_count[0]> (RAY_PACKET_BLOCK_SIZE- MAX_SECONDARY_RAY_COUNT))
#else
		if(secondary_ray_count[0]> (RAY_BLOCK_SIZE- MAX_SECONDARY_RAY_COUNT* PACKET_SIZE))
#endif
			break;
		packets_shaded_count[0]++;

		bool ray_is_active= !(ray_packets[i].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID || ray_packets[i].mask[programIndex]!= -1);
		cif(!ray_is_active)
			continue;

		bool is_primary= ray_packets[i].type[programIndex]< 2;

		float packet_x= ray_packets[i].x[programIndex];//name change
		float packet_y= ray_packets[i].y[programIndex];

		float<3> color= {0.0f, 0.0f, 0.0f};

		float<3> position= {ray_packets[i].surface_.position_x[programIndex], 
			                ray_packets[i].surface_.position_y[programIndex], 
						    ray_packets[i].surface_.position_z[programIndex]};

		bool in_air= ray_packets[i].medium.is_air[programIndex];
		float normal_direction= in_air ? 1.0f : -1.0f;
		float<3> normal= {ray_packets[i].surface_.normal_x[programIndex]* normal_direction, 
			              ray_packets[i].surface_.normal_y[programIndex]* normal_direction, 
						  ray_packets[i].surface_.normal_z[programIndex]* normal_direction};

		float<3> direction= {ray_packets[i].dirx[programIndex], 
							 ray_packets[i].diry[programIndex], 
							 ray_packets[i].dirz[programIndex]};
		float direction_magnitude= sqrt(direction.x* direction.x+ direction.y* direction.y+ direction.z* direction.z);
		direction.x/= direction_magnitude;//need to determine if normalization is necessary
		direction.y/= direction_magnitude;
		direction.z/= direction_magnitude;

		int bounce_count= ray_packets[i].bounce_count[programIndex];
		bool bounce_limit_achieved= bounce_count>= MAX_BOUNCE_COUNT;

		float<3> surface_diffuse_color;
		float<3> surface_specular_color;
		float reflectivity;
		float transmittance;
		float transmitted_refractive_index;//name change

		int material_id= ray_packets[i].surface_.material_id[programIndex];
		uniform int uniform_material_id;
		uniform bool material_is_uniform= reduce_equal(material_id, &uniform_material_id);
		if(material_is_uniform)
		{
			surface_diffuse_color.r= materials[uniform_material_id].diffuse[0];
			surface_diffuse_color.g= materials[uniform_material_id].diffuse[1];
			surface_diffuse_color.b= materials[uniform_material_id].diffuse[2];

			surface_specular_color.r= materials[uniform_material_id].specular[0];
			surface_specular_color.g= materials[uniform_material_id].specular[1];
			surface_specular_color.b= materials[uniform_material_id].specular[2];

			reflectivity= materials[uniform_material_id].reflectivity;
			transmittance= materials[uniform_material_id].transparency;

			transmitted_refractive_index= materials[uniform_material_id].refractive_index;
		}
		else
		{
			foreach_unique(unique_material_id in material_id)
			{
				surface_diffuse_color.r= materials[unique_material_id].diffuse[0];
				surface_diffuse_color.g= materials[unique_material_id].diffuse[1];
				surface_diffuse_color.b= materials[unique_material_id].diffuse[2];

				surface_specular_color.r= materials[unique_material_id].specular[0];
				surface_specular_color.g= materials[unique_material_id].specular[1];
				surface_specular_color.b= materials[unique_material_id].specular[2];

				reflectivity= materials[unique_material_id].reflectivity;
				transmittance= materials[unique_material_id].transparency;

				transmitted_refractive_index= materials[unique_material_id].refractive_index;
			}
		}
		float reflectivity_complement= 1- reflectivity;
		float transmittance_complement= 1- transmittance;
		float glass_complement= 1- transmittance- reflectivity;
		bool glass_interaction= (glass_complement)< 0.001f;


		cif(!bounce_limit_achieved)
		{
			if(!in_air)
				transmitted_refractive_index= 1.0f;
			float incoming_refractive_index= ray_packets[i].medium.refractive_index[programIndex];//name change
			float cos_incoming= -(direction.x* normal.x+ 
									direction.y* normal.y+ 
									direction.z* normal.z);

			float push= 0.015f;
			float<3> reflection_position= {position[0]+ normal[0]* push, 
											position[1]+ normal[1]* push, 
											position[2]+ normal[2]* push};

			float diffuse_color_magnitude= 0.223f* surface_diffuse_color.r+ 
											0.461f* surface_diffuse_color.g+ 
											0.316f* surface_diffuse_color.b;

#if RUSSIAN_ROULETTE
			float interval_sample= (interval_samples+ (interval_sample_indices[i]+ frame_count)* PACKET_SIZE)[programIndex];

			
			float diffuse_threshold= diffuse_color_magnitude* glass_complement;
			bool diffuse_interaction= interval_sample< diffuse_threshold;

			float adjusted_diffuse_term;
			if(bounce_count>= RUSSIAN_ROULETTE_BOUNCE)
				adjusted_diffuse_term= diffuse_interaction ? 1.0f : 0.0f;
			else
				adjusted_diffuse_term= diffuse_color_magnitude* glass_complement;

			bool do_diffuse= adjusted_diffuse_term> 0.001f;
			

#else
			float adjusted_diffuse_term= diffuse_color_magnitude* glass_complement;
			bool do_diffuse= !glass_interaction;
#endif

			if(do_diffuse)
			{
				float<3> normalized_diffuse_color= {surface_diffuse_color.r/ diffuse_color_magnitude, 
					                                surface_diffuse_color.g/ diffuse_color_magnitude, 
													surface_diffuse_color.b/ diffuse_color_magnitude};

				float absorption_r= ray_packets[i].absorption_r[programIndex]* normalized_diffuse_color.r* adjusted_diffuse_term;
				float absorption_g= ray_packets[i].absorption_g[programIndex]* normalized_diffuse_color.g* adjusted_diffuse_term;
				float absorption_b= ray_packets[i].absorption_b[programIndex]* normalized_diffuse_color.b* adjusted_diffuse_term;

				float<3> reflection_direction;
				SampleHemisphere_GeometryImportance(disc_sample_indices[i]+ frame_count,
													normal.x, normal.y, normal.z,
													&(reflection_direction.x), &(reflection_direction.y), &(reflection_direction.z),
													primary_disc_samples);

#if DATA_COLLECTION
				LogData_Shading_Reflection(data,
											normal.x, normal.y, normal.z, 
											reflection_direction.x, reflection_direction.y, reflection_direction.z, 
											ray_packets[i].x[programIndex], ray_packets[i].y[programIndex],
											bounce_count, 
											ray_packets[i].type[programIndex],
											5);
#endif

#if PACKETED_SECONDARY_RAYS
				WriteRayPacket_Coherent(secondary_ray_packets, secondary_packet_count, &secondary_ray_count, 
								!bounce_limit_achieved && do_diffuse && ray_is_active, 
								reflection_position, reflection_direction, 0.0f, 1000000,
								packet_x, packet_y, 
								bounce_count+ 1, 5,
								incoming_refractive_index, in_air,
								absorption_r, absorption_g, absorption_b);
#else
				not implemented
#endif
			}


			float fresnel_reflectivity= 0.0f;
			float fresnel_transmittance= 0.0f;

#if RUSSIAN_ROULETTE
			float refraction_threshold= diffuse_threshold+ transmittance;
			
			bool refractive_interaction= interval_sample< refraction_threshold && !diffuse_interaction;
			bool do_refraction= refractive_interaction;

#else
			bool do_refraction= transmittance> 0;
#endif

			if(do_refraction)
			{
				//print("refraction\n");
				float index_ratio= incoming_refractive_index/ transmitted_refractive_index;
				float sin_transmitted_squared= (index_ratio* index_ratio)* (1- cos_incoming* cos_incoming);

				cif(sin_transmitted_squared> 1)
					reflectivity+= transmittance;
				else
				{
					float cos_transmitted= sqrt(1- sin_transmitted_squared);

					float r0= (incoming_refractive_index- transmitted_refractive_index)/
								(incoming_refractive_index+ transmitted_refractive_index);
					r0= r0* r0;
					float cos_term= incoming_refractive_index<= transmitted_refractive_index ? cos_incoming : cos_transmitted;
					float reflection_coefficient= r0+ (1- r0)* pow(1- cos_term, 5);

					fresnel_reflectivity= reflection_coefficient;
					fresnel_transmittance= 1- reflection_coefficient;

					float push= -0.015f;
					float<3> refraction_position= {position[0]+ normal[0]* push, 
													position[1]+ normal[1]* push, 
													position[2]+ normal[2]* push};

					float normal_coefficient= index_ratio* cos_incoming- cos_transmitted;
					float<3> refraction_direction= {direction.x* index_ratio+ normal.x* normal_coefficient, 
													direction.y* index_ratio+ normal.y* normal_coefficient, 
													direction.z* index_ratio+ normal.z* normal_coefficient};

					float absorption_r= ray_packets[i].absorption_r[programIndex]* fresnel_transmittance;//Transparent objects colorless for now
					float absorption_g= ray_packets[i].absorption_g[programIndex]* fresnel_transmittance;
					float absorption_b= ray_packets[i].absorption_b[programIndex]* fresnel_transmittance;

					//Need way to get rid of these blocks
#if PACKETED_SECONDARY_RAYS
					WriteRayPacket_Coherent(secondary_ray_packets, secondary_packet_count, &secondary_ray_count,
									!bounce_limit_achieved && do_refraction && ray_is_active, //should just use activity_mask (in function)
									refraction_position, refraction_direction, 0.0f, 1000000, 
									packet_x, packet_y, 
									bounce_count+ 1, is_primary ? 2 : 4, 
									transmitted_refractive_index, !in_air,
									absorption_r, absorption_g, absorption_b);
#else
					not implemented
#endif
				}
			}


			float specular_color_magnitude= 0.223f* surface_specular_color.r+ 
											0.461f* surface_specular_color.g+ 
											0.316f* surface_specular_color.b;

#if RUSSIAN_ROULETTE
			float reflection_threshold= refraction_threshold+ specular_color_magnitude* reflectivity;
			bool reflective_interaction= interval_sample< reflection_threshold && !refractive_interaction && !diffuse_interaction;

			float adjusted_reflection_term= reflective_interaction ? 1.0f : fresnel_reflectivity;

			bool do_reflection= adjusted_reflection_term> 0;

#else
			float adjusted_reflection_term= reflectivity+ fresnel_reflectivity;
			bool do_reflection= adjusted_reflection_term> 0;
#endif

			if(do_reflection)
			{
				float normal_coefficient= cos_incoming* 2;

				float<3> reflection_direction= {direction.x+ normal.x* normal_coefficient, 
												direction.y+ normal.y* normal_coefficient, 
												direction.z+ normal.z* normal_coefficient};

				float absorption_r= ray_packets[i].absorption_r[programIndex]* (surface_specular_color.r/ specular_color_magnitude)* adjusted_reflection_term;
				float absorption_g= ray_packets[i].absorption_g[programIndex]* (surface_specular_color.g/ specular_color_magnitude)* adjusted_reflection_term;
				float absorption_b= ray_packets[i].absorption_b[programIndex]* (surface_specular_color.b/ specular_color_magnitude)* adjusted_reflection_term;

#if PACKETED_SECONDARY_RAYS
				WriteRayPacket_Coherent(secondary_ray_packets, secondary_packet_count, &secondary_ray_count, 
								!bounce_limit_achieved && do_reflection && ray_is_active,
								reflection_position, reflection_direction, 0.0f, 1000000, 
								packet_x, packet_y, 
								bounce_count+ 1, is_primary ? 1 : 3, 
								incoming_refractive_index, in_air,
								absorption_r, absorption_g, absorption_b);
#else
				not implemented
#endif
			}

			//if(!reflective_interaction && !refractive_interaction && !diffuse_interaction)
			//	print("absorbed\n");
		}

		cif(in_air && !glass_interaction)
		{
			for(uniform int j= 0; j< lighting->point_light_count; j++)
			{
#if 0
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
					color[0]+= (lighting->point_lights[j].intensity[0]* geometry_term);
					color[1]+= (lighting->point_lights[j].intensity[1]* geometry_term);
					color[2]+= (lighting->point_lights[j].intensity[2]* geometry_term);
				}

#else
				//Have to do it this way or ISPC compiler gets confused an orders a gather.
				float light_coefficient= (ray_packets[i].light_coefficients+ lighting->point_light_count* j)[programIndex];

				color[0]+= lighting->point_lights[j].intensity[0]* light_coefficient;
				color[1]+= lighting->point_lights[j].intensity[1]* light_coefficient;
				color[2]+= lighting->point_lights[j].intensity[2]* light_coefficient;
#endif
			}
		
			color[0]*= surface_diffuse_color.r;
			color[1]*= surface_diffuse_color.g;
			color[2]*= surface_diffuse_color.b;

			color[0]+= lighting->ambient[0];
			color[1]+= lighting->ambient[1];
			color[2]+= lighting->ambient[2];

			color[0]*= ray_packets[i].absorption_r[programIndex]* glass_complement;
			color[1]*= ray_packets[i].absorption_g[programIndex]* glass_complement;
			color[2]*= ray_packets[i].absorption_b[programIndex]* glass_complement;

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
#if INDIRECT_LIGHTING_ONLY
			if(bounce_count> 0)
#endif
			{
				int receptor_index= packet_y* film_width+ packet_x;

				foreach_unique(unique_receptor_index in receptor_index)
				{
					uniform float<3> color_sum= {reduce_add(color[0]), reduce_add(color[1]), reduce_add(color[2])};

					receptors_r[unique_receptor_index]+= color_sum[0];
					receptors_g[unique_receptor_index]+= color_sum[1];
					receptors_b[unique_receptor_index]+= color_sum[2];
				}
			}

#if ADAPTIVE_SAMPLING
			if(noisy_receptors!= NULL)
			{
				uniform float<3> color_mean= color_sum/ programCount;
				float<3> displacement= (color- color_mean);//try using intrisic to square resulting vector
				float squared_distance= displacement[0]* displacement[0]+ displacement[1]* displacement[1]+ displacement[2]* displacement[2];

				uniform float variance= reduce_add(squared_distance);
				if(variance> COLOR_VARIANCE_THRESHOLD)
				{
					//This is technically not tested in its current state
					//May not need global?
					noisy_receptors[atomic_add_global(noisy_receptor_count, 1)]= receptor_index;

					sample_counts[receptor_index]= MAX_SAMPLES_PER_PIXEL;
				}
				else
					sample_counts[receptor_index]= MIN_SAMPLES_PER_PIXEL;

			}
#endif
#endif
#endif
		}
	}

	if(secondary_ray_count> 0)
		secondary_ray_packets[secondary_packet_count[0]++].mask[programIndex]= (programIndex< secondary_ray_count) ? -1 : 0;
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

#if ISPC_INTERPOLATION
export void Interpolate_Coherent(uniform RayPacket ray_packets[], uniform int packet_count,
								uniform Mesh meshes[], uniform int material_ids[])
{
#if PACKET_MODE
	for(int i= 0; i< packet_count; i++)
	{
		cif(ray_packets[i].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID || ray_packets[i].mask[programIndex]!= -1)
			continue;

		ray_packets[i].surface_.position_x[programIndex]= ray_packets[i].orgx[programIndex]+ (ray_packets[i].dirx[programIndex]* ray_packets[i].tfar[programIndex]); 
		ray_packets[i].surface_.position_y[programIndex]= ray_packets[i].orgy[programIndex]+ (ray_packets[i].diry[programIndex]* ray_packets[i].tfar[programIndex]);
		ray_packets[i].surface_.position_z[programIndex]= ray_packets[i].orgz[programIndex]+ (ray_packets[i].dirz[programIndex]* ray_packets[i].tfar[programIndex]);

		float u= ray_packets[i].u[programIndex];
		float v= ray_packets[i].v[programIndex];
		float w= 1- u- v;

		//ugh, have to repeat code 4 times...
		int geometry_id= ray_packets[i].geomID[programIndex];
		uniform int uniform_geometry_id;
		if(reduce_equal(geometry_id, &uniform_geometry_id))
		{
			int * uniform normal_indices= meshes[uniform_geometry_id].normal_indices;
			float * uniform normals= meshes[uniform_geometry_id].normals;

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

				ray_packets[i].surface_.normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
				ray_packets[i].surface_.normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
				ray_packets[i].surface_.normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;

				ray_packets[i].surface_.material_id[programIndex]= material_ids[uniform_geometry_id];
			}
			else
			{
				foreach_unique(unique_primitive_id in primitive_id)
				{
					uniform int normal0_index= normal_indices[unique_primitive_id* 3+ 0];
					uniform int normal1_index= normal_indices[unique_primitive_id* 3+ 1];
					uniform int normal2_index= normal_indices[unique_primitive_id* 3+ 2];

					float<3> normal0= {normals[normal0_index* 3+ 0], normals[normal0_index* 3+ 1], normals[normal0_index* 3+ 2]};
					float<3> normal1= {normals[normal1_index* 3+ 0], normals[normal1_index* 3+ 1], normals[normal1_index* 3+ 2]};
					float<3> normal2= {normals[normal2_index* 3+ 0], normals[normal2_index* 3+ 1], normals[normal2_index* 3+ 2]};

					ray_packets[i].surface_.normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
					ray_packets[i].surface_.normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
					ray_packets[i].surface_.normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;

					ray_packets[i].surface_.material_id[programIndex]= material_ids[uniform_geometry_id];
				}
			}
		}
		else
		{
			foreach_unique(unique_geometry_id in ray_packets[i].geomID[programIndex])
			{
				int * uniform normal_indices= meshes[unique_geometry_id].normal_indices;
				float * uniform normals= meshes[unique_geometry_id].normals;

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

					ray_packets[i].surface_.normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
					ray_packets[i].surface_.normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
					ray_packets[i].surface_.normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;

					ray_packets[i].surface_.material_id[programIndex]= material_ids[unique_geometry_id];
				}
				else
				{
					foreach_unique(unique_primitive_id in primitive_id)
					{
						uniform int normal0_index= normal_indices[unique_primitive_id* 3+ 0];
						uniform int normal1_index= normal_indices[unique_primitive_id* 3+ 1];
						uniform int normal2_index= normal_indices[unique_primitive_id* 3+ 2];

						float<3> normal0= {normals[normal0_index* 3+ 0], normals[normal0_index* 3+ 1], normals[normal0_index* 3+ 2]};
						float<3> normal1= {normals[normal1_index* 3+ 0], normals[normal1_index* 3+ 1], normals[normal1_index* 3+ 2]};
						float<3> normal2= {normals[normal2_index* 3+ 0], normals[normal2_index* 3+ 1], normals[normal2_index* 3+ 2]};

						ray_packets[i].surface_.normal_x[programIndex]= normal0[0]* w+ normal1[0]* u+ normal2[0]* v;
						ray_packets[i].surface_.normal_y[programIndex]= normal0[1]* w+ normal1[1]* u+ normal2[1]* v;
						ray_packets[i].surface_.normal_z[programIndex]= normal0[2]* w+ normal1[2]* u+ normal2[2]* v;

						ray_packets[i].surface_.material_id[programIndex]= material_ids[unique_geometry_id];
					}
				}
			}
		}
	}
#else
#endif
}

#endif

export void ComputeLightCoefficients_Coherent(uniform RayPacket ray_packets[], 
											uniform int packet_count,
											Lighting * uniform lighting, 
											uniform VisibilityRayPacket shadow_ray_packets[],
											RTCScene * uniform embree_scene)
{
	uniform int packet_indices[RAY_PACKET_BLOCK_SIZE];
	uniform int shadow_packet_count;
	uniform float geometry_terms[RAY_PACKET_BLOCK_SIZE* PACKET_SIZE];

	for(uniform int light_index= 0; light_index< lighting->point_light_count; light_index++)
	{
		uniform float light_position[3]= lighting->point_lights[light_index].position;

		shadow_packet_count= 0;
		for(uniform int packet_index= 0; packet_index< packet_count; packet_index++)
		{
			cif(ray_packets[packet_index].geomID[programIndex]== RTC_INVALID_GEOMETRY_ID  || ray_packets[packet_index].mask[programIndex]!= -1)
				continue;

			//uniform float * uniform light_coefficients= ray_packets[packet_index].light_coefficients+ light_index* PACKET_SIZE;

			cif(!ray_packets[packet_index].medium.is_air[programIndex])
				continue;

			float<3> surface_normal= {ray_packets[packet_index].surface_.normal_x[programIndex], 
			                          ray_packets[packet_index].surface_.normal_y[programIndex],
			                          ray_packets[packet_index].surface_.normal_z[programIndex]};

			float<3> surface_position= {ray_packets[packet_index].surface_.position_x[programIndex],
			                            ray_packets[packet_index].surface_.position_y[programIndex],
										ray_packets[packet_index].surface_.position_z[programIndex]};

			//Assuming we add surface position to ray extras via Interpolate
			float<3> direction= {light_position[0]- surface_position[0], 
				                 light_position[1]- surface_position[1],
								 light_position[2]- surface_position[2]};
			float direction_magnitude= sqrt(direction[0]* direction[0]+ 
											 direction[1]* direction[1]+
											 direction[2]* direction[2]);
			direction[0]/= direction_magnitude;
			direction[1]/= direction_magnitude;
			direction[2]/= direction_magnitude;

			float geometry_term= (direction[0]* surface_normal[0]+ 
				                 direction[1]* surface_normal[1]+ 
								 direction[2]* surface_normal[2]);

			if(all(geometry_term< 0))
			{
				(ray_packets[packet_index].light_coefficients+ light_index* PACKET_SIZE)[programIndex]= 0.0f;
				continue;
			}


			uniform int shadow_packet_index= shadow_packet_count++;

			cif(geometry_term< 0)
			{
				geometry_term= 0.0f;
				shadow_ray_packets[shadow_packet_index].mask[programIndex]= 0;
			}
			else
				shadow_ray_packets[shadow_packet_index].mask[programIndex]= -1;

			cif(geometry_term< 0.1f)
			{
				shadow_ray_packets[shadow_packet_index].orgx[programIndex]= surface_position[0]+ surface_normal[0]* 0.005f;
				shadow_ray_packets[shadow_packet_index].orgy[programIndex]= surface_position[1]+ surface_normal[1]* 0.005f;
				shadow_ray_packets[shadow_packet_index].orgz[programIndex]= surface_position[2]+ surface_normal[2]* 0.005f;
			}
			else
			{
				shadow_ray_packets[shadow_packet_index].orgx[programIndex]= surface_position[0]+ surface_normal[0]* 0.001f;
				shadow_ray_packets[shadow_packet_index].orgy[programIndex]= surface_position[1]+ surface_normal[1]* 0.001f;
				shadow_ray_packets[shadow_packet_index].orgz[programIndex]= surface_position[2]+ surface_normal[2]* 0.001f;
			}

			geometry_terms[packet_index* PACKET_SIZE+ programIndex]= geometry_term;

			shadow_ray_packets[shadow_packet_index].dirx[programIndex]= direction[0];
			shadow_ray_packets[shadow_packet_index].diry[programIndex]= direction[1];
			shadow_ray_packets[shadow_packet_index].dirz[programIndex]= direction[2];

			shadow_ray_packets[shadow_packet_index].tnear[programIndex]= 0.0f;
			shadow_ray_packets[shadow_packet_index].tfar[programIndex]= direction_magnitude;
			shadow_ray_packets[shadow_packet_index].geomID[programIndex]= RTC_INVALID_GEOMETRY_ID;
			shadow_ray_packets[shadow_packet_index].primID[programIndex]= RTC_INVALID_GEOMETRY_ID;
			shadow_ray_packets[shadow_packet_index].time[programIndex]= 0;


			packet_indices[shadow_packet_index]= packet_index;
		}

		//Should try with single packets
		uniform RTCIntersectContext context;
		context.flags= RTC_INTERSECT_COHERENT;
		context.userRayExt= NULL;
		rtcOccludedNM(*embree_scene, &context, (RTCRayN *)shadow_ray_packets, PACKET_SIZE, shadow_packet_count, sizeof(VisibilityRayPacket)/ 8);

		for(uniform int shadow_packet_index= 0; shadow_packet_index< shadow_packet_count; shadow_packet_index++)
		{
			uniform int packet_index= packet_indices[shadow_packet_index];

			float light_coefficient;
			cif(shadow_ray_packets[shadow_packet_index].geomID[programIndex]== 0)
				light_coefficient= 0.0f;
			else
				light_coefficient= geometry_terms[packet_index* PACKET_SIZE+ programIndex];

			(ray_packets[packet_index].light_coefficients+ lighting->point_light_count* light_index)[programIndex]= light_coefficient;
		}
	}
}

export void GenerateCompositingMaps(uniform RayPacket ray_packets[], 
									uniform int packet_count, 
									uniform Mesh meshes[],
									uniform PhongMaterial materials[],	
									uniform float camera_transform[],
									uniform unsigned int8 diffuse_map[],
									uniform unsigned int8 glossiness_map[],
									uniform unsigned int8 normal_map[],
									uniform float depth_map[],
									uniform int map_width)
{
	for(uniform int i= 0; i< packet_count; i++)
	{
		uniform int x= extract(ray_packets[i].x[programIndex], 0);
		uniform int y= extract(ray_packets[i].y[programIndex], 0);

		int mesh_id= ray_packets[i].geomID[programIndex];

		uniform int uniform_mesh_id;
		if(reduce_equal(mesh_id, &uniform_mesh_id))
		{
			if(uniform_mesh_id== RTC_INVALID_GEOMETRY_ID || meshes[uniform_mesh_id].indirect_only)
				continue;
		}
		else
		{
			if(mesh_id== RTC_INVALID_GEOMETRY_ID || meshes[mesh_id].indirect_only)
				continue;
		}

		float<3> position= {ray_packets[i].surface_.position_x[programIndex], 
			                ray_packets[i].surface_.position_y[programIndex], 
						    ray_packets[i].surface_.position_z[programIndex]};

		float<3> normal= {ray_packets[i].surface_.normal_x[programIndex], 
			              ray_packets[i].surface_.normal_y[programIndex], 
						  ray_packets[i].surface_.normal_z[programIndex]};

		float<3> surface_diffuse_color;
		float surface_glossiness;

		int material_id= ray_packets[i].surface_.material_id[programIndex];
		uniform int uniform_material_id;
		uniform bool material_is_uniform= reduce_equal(material_id, &uniform_material_id);
		if(material_is_uniform)
		{
			surface_diffuse_color.r= materials[uniform_material_id].diffuse[0];
			surface_diffuse_color.g= materials[uniform_material_id].diffuse[1];
			surface_diffuse_color.b= materials[uniform_material_id].diffuse[2];
			surface_glossiness= materials[uniform_material_id].glossiness;
		}
		else
		{
			foreach_unique(unique_material_id in material_id)
			{
				surface_diffuse_color.r= materials[unique_material_id].diffuse[0];
				surface_diffuse_color.g= materials[unique_material_id].diffuse[1];
				surface_diffuse_color.b= materials[unique_material_id].diffuse[2];
				surface_glossiness= materials[unique_material_id].glossiness;
			}
		}

		int map_index= (y* map_width+ x)+ programIndex;

		diffuse_map[map_index* 4+ 0]= (int)(255* surface_diffuse_color.b);
		diffuse_map[map_index* 4+ 1]= (int)(255* surface_diffuse_color.g);
		diffuse_map[map_index* 4+ 2]= (int)(255* surface_diffuse_color.r);

		glossiness_map[map_index* 4+ 2]= (int)(255* (surface_glossiness/ 100.0f));

		normal_map[map_index* 4+ 0]= (int)(255* (normal.z+ 1)/ 2);
		normal_map[map_index* 4+ 1]= (int)(255* (normal.y+ 1)/ 2);
		normal_map[map_index* 4+ 2]= (int)(255* (normal.x+ 1)/ 2);

		float position_x= position.x;
		float position_y= position.y;
		float position_z= position.z;
		depth_map[map_index]= ((camera_transform[8]* position_x+ 
											camera_transform[9]* position_y+ 
											camera_transform[10]* position_z+ 
											camera_transform[11])/ 
											(camera_transform[12]* position_x+ 
											camera_transform[13]* position_y+ 
											camera_transform[14]* position_z+ 
											camera_transform[15])+ 1)/ 2;
	}
}