#include "OpenGLUtility.h"
#include "Common.h"

string TranslateGLError(GLenum error)
{
	switch(error)
	{
	case GL_NO_ERROR: return "GL_NO_ERROR";
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
	default: return "Error not found... today is just not your day :'(";
	}
}

void HandleGLErrors()
{
	GLenum error= glGetError();
	
	if(error== GL_NO_ERROR)
		return;

	cout << "OpenGL Error: " << TranslateGLError(error) << endl;
	assert(false && "OpenGL Error!");
}