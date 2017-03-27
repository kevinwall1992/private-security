#version 420

in vec2 texture_coordinates;

uniform sampler2D texture_;

uniform vec3 tint;

out vec4 color;

void main() 
{ 
  color= vec4(tint, 1)* texture(texture_, texture_coordinates);
}
