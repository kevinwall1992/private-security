#include "VAO.h"
#include "Common.h"
#include "Shader.h"


VAO VAO::default_vao(true);
VAO VAO::current_vao= VAO::default_vao;

VAO::VAO(bool is_default)
{
	if(is_default)
		handle= 0;
	else
		handle= -1;
}

VAO::VAO()
{
	handle= -1;
}

GLuint VAO::GetHandle()
{
	return handle;
}

void VAO::Bind()
{
	if(handle== -1)
		glGenVertexArrays(1, &handle);

	if(current_vao.GetHandle()== handle)
		return;

	glBindVertexArray(handle);
	current_vao= *this;
}

//The reinterpret cast may not work, currently don't use strided buffers
void VAO::SetAttributeBindingf(GLuint attribute_index, int size, int stride, int offset)
{
	assert(attribute_index!= -1 && "ShaderProgram::SetAttribute: Attribute not found.");

	Bind();

	glVertexAttribPointer(attribute_index, size, GL_FLOAT, GL_FALSE, sizeof(float)* stride, reinterpret_cast<void *>((size_t)offset));
	glEnableVertexAttribArray(attribute_index);
}

void VAO::SetAttributeBinding3f(GLuint attribute_index)
{
	SetAttributeBindingf(attribute_index, 3, 3, 0);
}

void VAO::SetAttributeBinding2f(GLuint attribute_index)
{
	SetAttributeBindingf(attribute_index, 2, 2, 0);
}

void VAO::Free()
{
	glDeleteVertexArrays(1, &handle);
}

VAO VAO::GetDefaultVAO()
{
	return default_vao;
}

VAO VAO::GetCurrentVAO()
{
	return current_vao;
}
