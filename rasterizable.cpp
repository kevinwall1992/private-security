#include "Rasterizable.h"


float FullScreenQuad::vertices[18]= {-1.0f, -1.0f, +0.0f, +1.0f, -1.0f, +0.0f, -1.0f, +1.0f, +0.0f, 
					                 +1.0f, -1.0f, +0.0f, +1.0f, +1.0f, +0.0f, -1.0f, +1.0f, +0.0f};
VAO FullScreenQuad::vao;
FullScreenQuad FullScreenQuad::instance;

FullScreenQuad::FullScreenQuad()
{

}

void FullScreenQuad::Rasterize()
{
	static bool initialized= false;
	if(!initialized)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 18* sizeof(float), vertices, GL_STATIC_DRAW);
		vao.SetAttributeBinding3f(0);

		initialized= true;
	}

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 18);
}

void FullScreenQuad::RasterizeInstance()
{
	instance.Rasterize();
}

void RasterizeFullScreenQuad()
{
	FullScreenQuad::RasterizeInstance();
}
