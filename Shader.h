#ifndef PS_SHADER
#define PS_SHADER

#include "GraphicsLibraries.h"
#include "FileResource.h"
#include "Transform.h"


GLuint CreateRawShader(GLenum shader_type, const char *shader_source, const char *name= nullptr);
GLuint CreateRawShaderProgram(GLuint vertex_shader, GLuint fragment_shader);

template<class T>
class Shader : public FileResource<T>
{
	GLuint shader_handle;

protected:
	Shader(string filepath, GLenum shader_type)
		: FileResource<T>(filepath)
	{
		shader_handle= CreateRawShader(shader_type, Utility::TextFileToString(filepath).c_str());
	}

public:
	GLuint GetHandle()
	{
		return shader_handle;
	}

	static string MakeFilepath(string filename)
	{
		return "data/shaders/"+ filename;
	}
};

class VertexShader : public Shader<VertexShader>
{
protected:
	VertexShader(string filepath);

public:
	static vector<VertexShader *> Parse(string filename);
};

class FragmentShader : public Shader<FragmentShader>
{
protected:
	FragmentShader(string filepath);

public:
	static vector<FragmentShader *> Parse(string filename);
};

class ShaderProgram : public FileResource<ShaderProgram>
{
	static ShaderProgram *current_shader_program;

	GLuint shader_program_handle;

	VertexShader *vertex_shader;
	FragmentShader *fragment_shader;

protected:
	ShaderProgram(string filepath, VertexShader *vertex_shader, FragmentShader *fragment_shader);

public:
	~ShaderProgram();

	GLuint GetHandle();

	GLuint GetAttributeLocation(string attribute_name);
	GLuint GetUniformLocation(string uniform_name);

	void SetUniformFloat(string uniform_name, float value);
	void SetUniformInt(string uniform_name, int value);
	void SetUniformVector2f(string uniform_name, Vec2f vector_);
	void SetUniformVector3f(string uniform_name, Vec3f vector_);
	void SetUniformMatrix4x4f(string uniform_name, Mat4x4f matrix);
	void SetUniformMatrix4x4f(string uniform_name, Transform transform);

	void Use();

	static ShaderProgram * GetCurrentProgram();
	static string MakeFilepath(string filename);
	static vector<ShaderProgram *> Parse(string filename);
};

#endif
