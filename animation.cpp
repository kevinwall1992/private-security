#include "Animation.h"
#include "Shader.h"


Animation::Animation(Mesh *mesh, string action_name_, float cycles_)
	: mesh_prop(mesh)
{
	action_name= action_name_;
	cycles= cycles_;
}

void Animation::SetDisplacement(Vec3f displacement)
{
	mesh_prop.SetDisplacement(displacement);
}

void Animation::SetRotation(float rotation)
{
	mesh_prop.SetRotation(rotation);
}

void Animation::SetMoment(float moment_)
{
	moment= moment_;
}

Mesh * Animation::GetMesh()
{
	return mesh_prop.GetMesh();
}

vector<RaytracingPrimitive *> Animation::GetRaytracingPrimitives()
{
	if(AreDrawFlagsActive_All(DrawFlags::Indirect))
		return mesh_prop.GetRaytracingPrimitives();
	else
		return vector<RaytracingPrimitive *>();
}

void Animation::Rasterize()
{
	bool use_bones= action_name!= "";

	if(use_bones)
	{
		VAO vao= mesh_prop.GetVAO();

		GLuint bone_buffer_handle;
		glGenBuffers(1, &bone_buffer_handle);
		glBindBuffer(GL_ARRAY_BUFFER, bone_buffer_handle);
		glBufferData(GL_ARRAY_BUFFER, mesh_prop.GetMesh()->GetVertexCount()* 2* sizeof(int), &mesh_prop.GetMesh()->skin.vertex_bone_indices[0], GL_STATIC_DRAW);
		vao.SetAttributeBinding2f(ShaderProgram::GetCurrentProgram()->GetAttributeLocation("bone_indices"));
				
		GLuint weight_buffer_handle;
		glGenBuffers(1, &weight_buffer_handle);
		glBindBuffer(GL_ARRAY_BUFFER, weight_buffer_handle);
		glBufferData(GL_ARRAY_BUFFER, mesh_prop.GetMesh()->GetVertexCount()* 2* sizeof(float), &mesh_prop.GetMesh()->skin.vertex_weights[0], GL_STATIC_DRAW);
		vao.SetAttributeBinding2f(ShaderProgram::GetCurrentProgram()->GetAttributeLocation("weights"));
	}

	initialized= true;

	ShaderProgram::GetCurrentProgram()->SetUniformInt("use_bones", use_bones ? 1 : 0);
	
	if(use_bones)
	{
		Skeleton *skeleton= GetMesh()->skeleton;

		vector<Mat4x4f> bone_matrices;
		vector<Vector<3, float>> bone_heads;

		for(auto &bone_pair : GetMesh()->skin.bone_index_map)
		{
			Skeleton::Bone bone= skeleton->GetBone(bone_pair.first);

			Transform transform= skeleton->GetBoneTransform(bone.name, action_name, moment* cycles);
			bone_matrices.push_back(transform.GetMatrix());
			bone_heads.push_back(bone.head);
		}

		glUniformMatrix4fv(ShaderProgram::GetCurrentProgram()->GetUniformLocation("bone_matrices"), (GLsizei)bone_matrices.size(), GL_TRUE, (float *)(&bone_matrices[0]));
		glUniform3fv(ShaderProgram::GetCurrentProgram()->GetUniformLocation("bone_heads"), (GLsizei)bone_heads.size(), (float *)(&bone_heads[0]));
	}

	mesh_prop.SetDrawFlags(draw_flags);
	mesh_prop.Rasterize();
}