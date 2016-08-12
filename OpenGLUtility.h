#ifndef EBR_OPENGL_UTILITY
#define EBR_OPENGL_UTILITY

#include "GraphicsHeaders.h"

GLuint CreateShader(GLenum shader_type, const char *shader_source, const char *name= nullptr);
GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader);

#endif