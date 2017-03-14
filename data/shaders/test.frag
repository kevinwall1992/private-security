#version 420

uniform sampler2D texture_;

out vec4 color;


void main() 
{ 
  vec2 normalized_coordinates= gl_FragCoord.xy/ 1024;//textureSize(photo, 0);
  color= vec4((texture(texture_, normalized_coordinates).xyz- .9)* 10, 1);
}
