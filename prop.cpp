#include "Prop.h"
#include "Common.h"
#include "Shader.h"

#include <fstream>
#include <algorithm>
#include <map>

Prop::Prop()
{
	AddDrawFlags(DrawFlags::RasterizeGbuffers);
}

void Prop::AddDrawFlags(DrawFlags::Enum draw_flags_)
{
	draw_flags= (DrawFlags::Enum)(draw_flags | draw_flags_);
}

void Prop::RemoveDrawFlags(DrawFlags::Enum draw_flags_)
{
	draw_flags= (DrawFlags::Enum)(draw_flags ^ (draw_flags & draw_flags_));
}

bool Prop::AreDrawFlagsActive(DrawFlags::Enum draw_flags_)
{
	return (draw_flags & draw_flags_)== 0 ? false : true;
}

void Prop::RasterizeConditionally(DrawFlags::Enum draw_flags_)
{
	if(AreDrawFlagsActive(draw_flags_))
		Rasterize();
}

vector<RaytracingPrimitive *> PropContainer::GetRaytracingPrimitives()
{
	vector<RaytracingPrimitive *> primitives;

	vector<Prop *> props= GetProps();
	for(unsigned int i= 0; i< props.size(); i++)
	{
		vector<RaytracingPrimitive *> prop_primitives= props[i]->GetRaytracingPrimitives();

		//Consider writing some vector utility functions, and replacing this
		primitives.insert(primitives.end(), prop_primitives.begin(), prop_primitives.end()); 
	}

	return	primitives;
}

void PropContainer::Rasterize()
{
	vector<Prop *> props= GetProps();

	for(unsigned int i= 0; i< props.size(); i++)
		props[i]->Rasterize();
}

void PropContainer::RasterizeConditionally(DrawFlags::Enum draw_flags)
{
	if(!AreDrawFlagsActive(draw_flags))
		return;

	vector<Prop *> props= GetProps();

	for(unsigned int i= 0; i< props.size(); i++)
		props[i]->RasterizeConditionally(draw_flags);
}


vector<Prop *> BasicPropContainer::GetProps()
{
	return props;
}

void BasicPropContainer::AddProp(Prop *prop)
{
	props.push_back(prop);
}


void MeshProp::Initialize()
{
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);

	GLuint vertex_buffer_handle;
	glGenBuffers(1, &vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, mesh->GetVertexCount()* 3* sizeof(float), &mesh->positions[0], GL_STATIC_DRAW);

	ShaderProgram::GetCurrentProgram()->SetAttribute("position", 3, sizeof(float)* 3, 0);

	GLuint normal_buffer_handle;
	glGenBuffers(1, &normal_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, mesh->GetVertexCount()* 3* sizeof(float), &mesh->normals[0], GL_STATIC_DRAW);

	ShaderProgram::GetCurrentProgram()->SetAttribute("normal", 3, sizeof(float)* 3, 0);

	glGenBuffers(1, &element_buffer_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetTriangleCount()* 3* sizeof(float), &mesh->position_indices[0], GL_STATIC_DRAW);

	is_initialized= true;
}

MeshProp::MeshProp(Mesh *mesh_)
{
	mesh= mesh_;
}

void MeshProp::SetDisplacement(Vec3f displacement_)
{
	displacement= displacement_;
}

void MeshProp::SetRotation(float rotation_)
{
	rotation= rotation_;
}

vector<RaytracingPrimitive *> MeshProp::GetRaytracingPrimitives()
{
	return MakeVector<RaytracingPrimitive *>(new RaytracingMesh(AreDrawFlagsActive(DrawFlags::RasterizeGbuffers) ? false : true, mesh));
}

void MeshProp::Rasterize()
{
	if(!is_initialized)
		Initialize();

	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);

	PhongMaterial *material= dynamic_cast<PhongMaterial *>(mesh->material);

	ShaderProgram *shader_program= ShaderProgram::GetCurrentProgram();
	shader_program->SetUniformMatrix4x4f("model_transform", Transform().RotateAboutY(0).Translate(displacement));
	shader_program->SetUniformVector3f("material_diffuse", material->diffuse);
	shader_program->SetUniformFloat("material_glossiness", material->glossiness);
	
	glDrawElements(GL_TRIANGLES, mesh->GetTriangleCount()* 3, GL_UNSIGNED_INT, nullptr);
}
