#version 420

in VertexData 
{
  vec3 position;
  vec3 normal;
}vertex_data;

uniform vec3 material_diffuse;
uniform float material_glossiness;

layout(location= 0) out vec3 diffuse;//Get rid of layout
layout(location= 1) out vec3 glossiness;
layout(location= 2) out vec3 normal;


void main() 
{ 
  diffuse= material_diffuse;
  normal= (vertex_data.normal+ vec3(1))/ 2;
  glossiness= vec3(material_glossiness/ 100.0);
  //normal= abs(vertex_data.position)+ abs(vertex_data.normal)* 0.01; 
}
