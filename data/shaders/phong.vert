#version 420

in vec3 position;

//out vec2 texture_coordinates;

void main() 
{
  //texture_coordinates= (position.xy+ vec2(1.0, 1.0))/ 2.0;

  gl_Position= vec4(position.xyz, 1.0); 
}
