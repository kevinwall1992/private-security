#version 420

in vec2 texture_coordinates;
in vec2 normalized_fragment_position;

uniform sampler2D texture_;
uniform sampler2D mask;

uniform bool draw_white;
uniform vec3 tint;

out vec4 color;

void main() 
{ 
  float mask_value= texture(mask, normalized_fragment_position).w;

  if(draw_white)
    color= vec4(tint, mask_value);
  else
    color= texture(texture_, texture_coordinates)* vec4(tint, mask_value);
}
