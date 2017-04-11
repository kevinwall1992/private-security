#version 420

uniform sampler2D diffuse_buffer;
uniform sampler2D glossiness_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D depth_buffer;
uniform sampler2D indirect_buffer;
uniform sampler2DShadow shadow_map;

//in vec2 texture_coordinates;

uniform mat4 camera_transform;
uniform mat4 projection_transform;
uniform vec3 camera_position;
uniform bool camera_is_orthographic;

uniform vec3 light_position;
uniform vec3 light_intensity;

uniform mat4 shadow_camera_transform;

out vec4 color;


struct Material
{
  vec3 diffuse;
  vec3 specular;
  float glossiness;
};

struct Surface
{
  vec3 position;
  vec3 normal;

  Material material;
  float shadow;
};

Surface GetSurface(vec2 normalized_coordinates)
{
  Surface surface;

  float a= projection_transform[2].z;
  float b= projection_transform[3].z;
  
  float depth= texture(depth_buffer, normalized_coordinates).x;
  if(depth> 0.999999)
    discard;
  
  vec3 clip_position= vec3((normalized_coordinates.xy* 2)- vec2(1, 1), depth* 2- 1);
  vec4 projected_position;
  if(camera_is_orthographic)
    projected_position= vec4(clip_position, 1);
  else
  {  
    vec3 clip_position= vec3((normalized_coordinates.xy* 2)- vec2(1, 1), depth* 2- 1);
    float w= b/ (clip_position.z+ a);
    vec4 projected_position= vec4(clip_position* w, w);
  }
  surface.position= (inverse(camera_transform)* projected_position).xyz;

  surface.normal= texture(normal_buffer, normalized_coordinates).xyz* 2- vec3(1);

  surface.material.diffuse= texture(diffuse_buffer, normalized_coordinates).xyz;
  surface.material.specular= vec3(1, 1, 1);
  surface.material.glossiness= texture(glossiness_buffer, normalized_coordinates).x* 100.0;

  
  surface.shadow= 0;
  int sample_count= 8;

  for(int i= 0; i< sample_count; i++)
  {
    float progress= (i+ 0.5)/ sample_count;
    float offset= progress* 0.1/ sample_count- 0.1/ 2.0;

    vec4 shadow_camera_clip_position= shadow_camera_transform* vec4(surface.position+ surface.normal* 0.025, 1);
    vec3 shadow_camera_ndc= shadow_camera_clip_position.xyz/ shadow_camera_clip_position.w;
    shadow_camera_ndc= (shadow_camera_ndc+ 1)/ 2;
    if(shadow_camera_ndc.x< 0.001 || shadow_camera_ndc.x> 0.999 || shadow_camera_ndc.y< 0.001 || shadow_camera_ndc.y> 0.999 || shadow_camera_clip_position.z< 0.001)
      surface.shadow+= 1.0/ sample_count;
    else
    {
      shadow_camera_ndc.z-= 0.00001+ 0.00004* progress;
      surface.shadow+= texture(shadow_map, shadow_camera_ndc.xyz)/ sample_count;
    }
  }


  return surface;
}


void main() 
{ 
  vec2 normalized_coordinates= gl_FragCoord.xy/ textureSize(normal_buffer, 0);
  //float depth= texture(depth_buffer, normalized_coordinates).x;
  Surface surface= GetSurface(normalized_coordinates);

	vec3 to_camera= normalize(camera_position- surface.position);
  vec3 to_light= normalize(light_position- surface.position);
  float cos_theta_incoming= dot(to_camera, surface.normal);
  float geometry_term= dot(to_light, surface.normal);

  vec3 diffuse, specular;
  diffuse= vec3(0);
  specular= vec3(0);

	if(geometry_term> 0)
	{
		diffuse= surface.material.diffuse* light_intensity* geometry_term* surface.shadow;

	  vec3 halfway_vector= (to_light+ to_camera)/ length(to_light+ to_camera);
	  specular= light_intensity* 
              pow(dot(surface.normal, halfway_vector), surface.material.glossiness)* 
              geometry_term* 
              surface.material.specular*
              surface.shadow;
	}

  color= vec4(vec3(1)* 0.0+ diffuse* 1.00+ specular* 1.0+ 1.0* texture(indirect_buffer, normalized_coordinates).xyz, 1);
  //final_color= vec4(vec3(depth- 0.995)* 200, 1);
  //final_color= vec4(vec3(texture(shadow_map, normalized_coordinates).x- 0.995)* 200, 1);
  //final_color= vec4(highlight ? 1.0f : 0.0f, vec2(texture(shadow_map, shadow_camera_ndc.xy).x- 0.995)* 200, 1);
  //final_color= vec4(surface.material.diffuse, 1);
  //final_color= vec4(abs(surface.normal), 1);
  //color= vec4(abs(surface.position)/ 5, 1);
  //final_color= vec4(vec3(surface.shadow), 1);
  //final_color= vec4(vec3(surface.material.glossiness/ 100), 1);
}
