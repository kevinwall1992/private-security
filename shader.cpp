#include "Shader.h"

#include <tinyxml.h>


GLuint CreateRawShader(GLenum shader_type, const char *shader_source, const char *name)
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

	HandleOpenGLErrors();

	return shader;
}

GLuint CreateRawShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
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
	glDetachShader(shader_program, fragment_shader);

	HandleOpenGLErrors();

	return shader_program;
}

VertexShader::VertexShader(string filepath)
	: Shader(filepath, GL_VERTEX_SHADER)
{

}

vector<VertexShader *> VertexShader::Parse(string filename)
{
	return Utility::MakeVector(new VertexShader(MakeFilepath(filename)));
}

FragmentShader::FragmentShader(string filepath)
	: Shader(filepath, GL_FRAGMENT_SHADER)
{

}

vector<FragmentShader *> FragmentShader::Parse(string filename)
{
	return Utility::MakeVector(new FragmentShader(MakeFilepath(filename)));
}


ShaderProgram * ShaderProgram::current_shader_program;

ShaderProgram::ShaderProgram(string filepath, VertexShader *vertex_shader_, FragmentShader *fragment_shader_)
	: FileResource<ShaderProgram>(filepath)
{
	vertex_shader= vertex_shader_;
	fragment_shader= fragment_shader_;

	shader_program_handle= CreateRawShaderProgram(vertex_shader->GetHandle(), fragment_shader->GetHandle());
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(shader_program_handle);
}

GLuint ShaderProgram::GetHandle()
{
	return shader_program_handle;
}

GLuint ShaderProgram::GetAttributeLocation(string attribute_name)
{
	return glGetAttribLocation(shader_program_handle, attribute_name.c_str());
}

GLuint ShaderProgram::GetUniformLocation(string uniform_name)
{
	return glGetUniformLocation(shader_program_handle, uniform_name.c_str());
}

void ShaderProgram::SetUniformFloat(string uniform_name, float value)
{
	glUniform1f(GetUniformLocation(uniform_name), value);
}

void ShaderProgram::SetUniformInt(string uniform_name, int value)
{
	glUniform1i(GetUniformLocation(uniform_name), value);
}

void ShaderProgram::SetUniformVector2f(string uniform_name, Vec2f vector_)
{
	glUniform2f(GetUniformLocation(uniform_name), vector_.x, vector_.y);
}

void ShaderProgram::SetUniformVector3f(string uniform_name, Vec3f vector_)
{
	glUniform3f(GetUniformLocation(uniform_name), vector_.x, vector_.y, vector_.z);
}

void ShaderProgram::SetUniformMatrix4x4f(string uniform_name, Mat4x4f matrix)
{
	glUniformMatrix4fv(GetUniformLocation(uniform_name), 1, GL_TRUE, matrix.c);
}

void ShaderProgram::SetUniformMatrix4x4f(string uniform_name, Transform transform)
{
	SetUniformMatrix4x4f(uniform_name, transform.GetMatrix());
}

void ShaderProgram::Use()
{
	glUseProgram(shader_program_handle);
	current_shader_program= this;
}

ShaderProgram * ShaderProgram::GetCurrentProgram()
{
	return current_shader_program;
}

string ShaderProgram::MakeFilepath(string filename)
{
	return "data/shaders/"+ filename;
}

vector<ShaderProgram*> ShaderProgram::Parse(string filename)
{
	VertexShader *vertex_shader= nullptr;
	FragmentShader *fragment_shader= nullptr;

	string filepath= MakeFilepath(filename);
	TiXmlDocument shader_program_file;
	shader_program_file.LoadFile(filepath.c_str());

	TiXmlElement *shaders_element= shader_program_file.RootElement();
	vertex_shader= VertexShader::Retrieve(shaders_element->FirstChildElement("vertex")->GetText());
	fragment_shader= FragmentShader::Retrieve(shaders_element->FirstChildElement("fragment")->GetText());

	return Utility::MakeVector(new ShaderProgram(filepath, vertex_shader, fragment_shader));
}
