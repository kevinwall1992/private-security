#version 420

in VertexData 
{
  vec3 position;
  vec3 normal;
}vertex_data;

uniform vec3 material_diffuse;
uniform float material_glossiness;

uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_intensity;

out vec4 color;


void main() 
{ 
  float foo= dot(normalize(light_position- vertex_data.position), vertex_data.normal);

  color= vec4(material_diffuse* light_intensity* foo, 1);
  //color= vec4(vertex_data.normal, 1);
}
