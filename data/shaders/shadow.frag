#version 420

in float depth;

layout(location= 0) out vec4 color;

void main() 
{ 
  color= vec4(vec3(depth), 1);
}
