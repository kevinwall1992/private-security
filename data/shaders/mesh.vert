#version 420

in vec3 position;
in vec3 normal;
in ivec2 bone_indices;//try ivec2****
in vec2 weights;

out VertexData 
{
  vec3 position;
  vec3 normal;
}vertex_data;

uniform mat4 model_transform;
uniform mat4 camera_transform;

uniform vec3 bone_heads[30];
uniform mat4 bone_matrices[30];
uniform bool use_bones;

void main() 
{
  vec4 new_position= vec4(position, 1.0);
  vec4 new_normal= vec4(normal, 0.0);

  if(use_bones)
  {
    //int bone_index0= int(bone_indices.x+ 0.5f);
    //int bone_index1= int(bone_indices.y+ 0.5f);
    
    vec3 bone_relative_position0= position- bone_heads[bone_indices.x];
    vec3 bone_relative_position1= position- bone_heads[bone_indices.y];
    new_position= (bone_matrices[bone_indices.x]* vec4(bone_relative_position0, 1.0))* weights.x;
    new_position+= (bone_matrices[bone_indices.y]* vec4(bone_relative_position1, 1.0))* weights.y;

    new_normal= bone_matrices[bone_indices.x]* vec4(normal, 0.0)* weights.x;
    new_normal+= bone_matrices[bone_indices.y]* vec4(normal, 0.0)* weights.y;
  }

  vertex_data.normal= normalize((model_transform* new_normal).xyz);
  vertex_data.position= (model_transform* new_position).xyz;

  gl_Position= camera_transform* model_transform* new_position; 
}
