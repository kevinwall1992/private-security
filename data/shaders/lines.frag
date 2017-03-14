#version 420

uniform vec3 line_color;

out vec4 color;

void main()
{ 
  color= vec4(line_color, 1);
}
