#version 420

in vec3 position;

uniform mat4 model_transform;
uniform mat4 camera_transform;
uniform vec3 light_position;

out float depth;

void main() 
{
  depth= length(light_position- (model_transform* vec4(position.xyz, 1.0)).xyz)/ 80;
  gl_Position= camera_transform* model_transform* vec4(position.xyz, 1.0);
}
