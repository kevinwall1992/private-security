#version 420

in vec3 position;

uniform mat4 transform;
uniform mat4 texture_transform;

out vec2 texture_coordinates;
out vec2 normalized_fragment_position;

void main() 
{
  vec4 transformed_position= transform* vec4(position.xyz, 1.0);

  gl_Position= transformed_position;
  texture_coordinates= (texture_transform* vec4((position.xyz+ 1)/ 2, 1.0)).xy;
  normalized_fragment_position= ((transformed_position.xyz+ 1)/ 2).xy;
}
