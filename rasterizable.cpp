#include "Rasterizable.h"


float FullScreenQuad::vertices[18]= {-1.0f, -1.0f, +0.0f, +1.0f, -1.0f, +0.0f, -1.0f, +1.0f, +0.0f, 
					                 +1.0f, -1.0f, +0.0f, +1.0f, +1.0f, +0.0f, -1.0f, +1.0f, +0.0f};
GLuint FullScreenQuad::vao_handle;
FullScreenQuad FullScreenQuad::instance;

FullScreenQuad::FullScreenQuad()
{

}

void FullScreenQuad::Rasterize()
{
	static bool initialized= false;
	if(!initialized)
	{
		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 18* sizeof(float), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 3, 0);
		glEnableVertexAttribArray(0);

		initialized= true;
	}

	glBindVertexArray(vao_handle);
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
