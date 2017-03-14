#version 420

in vec3 position;

uniform mat4 transform;
uniform mat4 texture_transform;

out vec2 texture_coordinates;

void main() 
{
  gl_Position= transform* vec4(position.xyz, 1.0);
  texture_coordinates= (texture_transform* vec4((position.xyz+ 1)/ 2, 1.0)).xy;
}
