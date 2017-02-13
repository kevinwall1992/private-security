#include "GraphicsSystem.h"
#include "Common.h"
#include "OpenGLUtility.h"
#include "Shader.h"
#include "Camera.h"
#include "Shader.h"
#include "Framebuffer.h"


void GraphicsSystem::Initialize()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	window = SDL_CreateWindow("Private Security", 
								SDL_WINDOWPOS_CENTERED, 
								SDL_WINDOWPOS_CENTERED, 
								screen_width, 
								screen_height, 
								SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	opengl_context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(0);
	int glew_result= glewInit();
	if(glew_result!= 0)
		cout << "GlewInit() error, returned " << glew_result;
	else
		cout << "OpenGL context: " << glGetString(GL_VERSION);
	cout << "\n\n";

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	SDL_GL_SetSwapInterval(0);

    glClearColor(0.0f, 0.0f, 0.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	ShaderProgram *shader_program= ShaderProgram::Retrieve("photo.program");
	shader_program->Use();

	shader_program->SetAttribute("position", 3, sizeof(float)* 3, 0);
	shader_program->SetUniformInt("photo", 0);
}

void GraphicsSystem::Terminate()
{
	SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int GraphicsSystem::GetScreenWidth()
{
	return screen_width;
}

int GraphicsSystem::GetScreenHeight()
{
	return screen_height;
}

void GraphicsSystem::Display(Photo photo)
{
	Framebuffer::GetDefault().Bind();
	glViewport(0, 0, screen_width* 2, screen_height* 2);

	photo.GetTexture().BindToIndex(0);
	ShaderProgram::Retrieve("photo.program")->Use();

	RasterizeFullScreenQuad();
	SDL_GL_SwapWindow(System::graphics.window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	photo.Free();

	frame_count++;
}

/*void GraphicsSystem::Display()
{
	SDL_GL_SwapWindow(System::graphics.window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	frame_count++;
}*/

int GraphicsSystem::GetFrameCount()
{
	return frame_count;
}