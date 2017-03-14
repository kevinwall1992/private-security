#ifndef PS_VAO
#define PS_VAO

#include "GraphicsLibraries.h"

class VAO
{
	static VAO default_vao, current_vao;

	GLuint handle;

	VAO(bool is_default);

public:
	VAO();

	GLuint GetHandle();

	void Bind();

	void SetAttributeBindingf(GLuint attribute_index, int size, int stride, int offset);
	void SetAttributeBinding3f(GLuint attribute_index);

	void Free();

	static VAO GetDefaultVAO();
	static VAO GetCurrentVAO();
};

#endif