#version 420

in vec2 texture_coordinates;

uniform sampler2D texture_;

out vec4 color;

void main() 
{ 
  color= texture(texture_, texture_coordinates);
}
