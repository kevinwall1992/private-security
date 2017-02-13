#version 420

in vec3 position;
in vec3 normal;

out VertexData 
{
  vec3 position;
  vec3 normal;
}vertex_data;

uniform mat4 model_transform;
uniform mat4 camera_transform;

void main() 
{
  gl_Position= camera_transform* model_transform* vec4(position.xyz, 1.0); 
  //vertex_data.normal= normalize((transpose(inverse(model_transform))* vec4(normal, 1)).xyz);
  vertex_data.normal= normalize((model_transform* vec4(normal, 0)).xyz);
  vertex_data.position= (model_transform* vec4(position.xyz, 1.0)).xyz;
}
