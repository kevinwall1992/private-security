#version 420

uniform sampler2D photo;

out vec4 color;

void main() 
{ 
  vec2 normalized_coordinates= gl_FragCoord.xy/ 1024;//textureSize(photo, 0);

  color= vec4(texture(photo, normalized_coordinates).xyz, 1);
}
