#include "GraphicsSystem.h"
#include "Common.h"
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ShaderProgram *shader_program= ShaderProgram::Retrieve("quad.program");
	shader_program->SetUniformInt("mask", 8);
	ColorImage white_image(Vec2i(screen_width, screen_height), Pixel::White);
	mask= Texture(white_image, 8);
}

void GraphicsSystem::Terminate()
{
	SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

Vec2i GraphicsSystem::GetScreenSize()
{
	return Vec2i(screen_width, screen_height);
}

void GraphicsSystem::Display(Drawable *drawable)
{
	Framebuffer::GetDefaultFramebuffer().PrepareForDrawing();

	drawable->Draw();

	SDL_GL_SwapWindow(System::graphics.window);

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

void GraphicsSystem::MoveMouseTo(Vec2f position)
{
	Vec2i pixel_position= Vec2f(position.x, 1- position.y)* GetScreenSize();

	SDL_WarpMouseInWindow(window, pixel_position.x, pixel_position.y);
}
