#include "OpenGLUtility.h"
#include "Common.h"


GLuint CreateShader(GLenum shader_type, const char *shader_source, const char *name)
{
	GLuint shader= glCreateShader(shader_type);

	glShaderSource(shader, 1, &shader_source, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		GLchar *info_log = new GLchar[info_log_length + 1];
		glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

		cout << "Compilation failure in " << (name== nullptr ? "shader" : name) << ". " << info_log;
		delete[] info_log;
	}

	return shader;
}

GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint shader_program= glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	glLinkProgram(shader_program);

	GLint status;
	glGetProgramiv (shader_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_log_length;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_log_length);

		GLchar *info_log = new GLchar[info_log_length + 1];
		glGetProgramInfoLog(shader_program, info_log_length, NULL, info_log);
		cout << "Linking failure: " << info_log;
		delete[] info_log;
	}

	glDetachShader(shader_program, vertex_shader);
	glDeleteShader(vertex_shader);
	glDetachShader(shader_program, fragment_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}