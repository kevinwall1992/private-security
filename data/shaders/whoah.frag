#version 420

uniform sampler2D color_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D depth_buffer;
uniform sampler2D indirect_buffer;
uniform sampler2D shadow_map;

//in vec2 texture_coordinates;

uniform mat4 camera_transform;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_intensity;

uniform float a;
uniform float b;

uniform mat4 shadow_camera_transform;


layout(location= 0) out vec4 final_color;

void main() 
{ 
  vec2 normalized_coordinates= gl_FragCoord.xy/ textureSize(normal_buffer, 0);

  vec3 bar= vec3((normalized_coordinates.xy* 2)- vec2(1, 1), texture(depth_buffer, normalized_coordinates).x* 2- 1);
  float foo= b/ (bar.z+ a);
  vec4 foobar= vec4(bar* foo, foo);
  vec3 position= (inverse(camera_transform)* foobar).xyz;

  vec3 color= texture(color_buffer, normalized_coordinates).xyz;
  vec3 normal= texture(normal_buffer, normalized_coordinates).xyz* 2- vec3(1);

  vec3 to_camera= normalize(camera_position- position);
  vec3 to_light= normalize(light_position- position);

  float cos_theta_incoming= dot(to_camera, normal);
  float geometry_term= dot(to_light, normal);
  vec3 specular_reflectivity= vec3(1, 1, 1);

	vec3 ambient, diffuse, specular;
	ambient= color;
  diffuse= vec3(0);
  specular= vec3(0);
  
  bool shadowed;
  vec4 shadow_camera_clip_position= shadow_camera_transform* vec4(position, 1);
  vec3 shadow_camera_ndc= shadow_camera_clip_position.xyz/ shadow_camera_clip_position.w;
  if(shadow_camera_ndc.x< 0 || shadow_camera_ndc.x> 1 || shadow_camera_ndc.y< 0 || shadow_camera_ndc.z> 1)
    shadowed= false;
  else
    shadowed= shadow_camera_ndc.z> texture(shadow_map, shadow_camera_ndc.xy).x;

	if(geometry_term> 0 && !shadowed)
	{
		diffuse= color* light_intensity* geometry_term;

	  vec3 halfway_vector= (to_light+ to_camera)/ length(to_light+ to_camera);
	  specular= light_intensity* pow(dot(normal, halfway_vector), 20)* geometry_term* specular_reflectivity;
	}
  //else
  //  final_color= vec4(0, 0, 0, 1);

  //final_color= vec4(ambient* 0.0+ diffuse* 1.0+ specular* 1.0+ texture(indirect_buffer, normalized_coordinates).xyz, 1);
  final_color= vec4(vec3(texture(shadow_map, shadow_camera_ndc.xy).x- .995)* 200, 1);
}
