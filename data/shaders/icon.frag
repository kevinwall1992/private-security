#version 420

in VertexData 
{
  vec3 position;
  vec3 normal;
  vec2 texture_coordinates;
}vertex_data;

uniform vec3 material_diffuse;
uniform float material_glossiness;

uniform sampler2D diffuse_texture;
uniform bool use_diffuse_texture;

uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_intensity;

out vec4 color;


void main() 
{ 
  vec3 to_camera= normalize(camera_position- vertex_data.position);
  vec3 to_light= normalize(light_position- vertex_data.position);
  
  float geometry_term= dot(to_light, vertex_data.normal);

  vec3 diffuse_color= vec3(0);
  vec3 specular_color= vec3(0);

  if(geometry_term> 0)
  {
    if(use_diffuse_texture)
      diffuse_color= texture(diffuse_texture, vec2(vertex_data.texture_coordinates.x, vertex_data.texture_coordinates.y)).xyz;
    else
      diffuse_color= material_diffuse;

    specular_color= light_intensity* pow(dot(vertex_data.normal, (to_light+ to_camera)/ length(to_light+ to_camera)), material_glossiness)* geometry_term/ 3;
  }
  
  color= vec4(diffuse_color* light_intensity* geometry_term+ specular_color, 1);
}
