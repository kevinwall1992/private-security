#version 420

in VertexData 
{
  vec3 position;
  vec3 normal;
}vertex_data;

uniform vec3 material_diffuse;
uniform float material_glossiness;

out vec4 diffuse_color;
out vec4 glossiness;
out vec4 normal;


void main() 
{ 
  diffuse_color= vec4(material_diffuse, 1);
  normal= vec4((vertex_data.normal+ vec3(1))/ 2, 1);
  glossiness= vec4(vec3(material_glossiness/ 100.0), 1);
  //normal= vec4(abs(vertex_data.position)+ abs(vertex_data.normal)* 0.01, 1);
}
