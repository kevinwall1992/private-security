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

uniform bool show_indirect_light;

out vec4 diffuse_color;
out vec4 glossiness;
out vec4 normal;
out vec4 indirect_light_stencil;


void main() 
{
  if(use_diffuse_texture)
    diffuse_color= texture(diffuse_texture, vec2(vertex_data.texture_coordinates.x, vertex_data.texture_coordinates.y));
  else
    diffuse_color= vec4(material_diffuse, 1);

  normal= vec4((vertex_data.normal+ vec3(1))/ 2, 1);

  glossiness= vec4(vec3(material_glossiness/ 100.0), 1);

  if(show_indirect_light)
    indirect_light_stencil= vec4(1, 1, 1, 1);
  else
    indirect_light_stencil= vec4(0, 0, 0, 1);
}
