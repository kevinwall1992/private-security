#include "GraphicsSystem.h"
#include "Common.h"
#include "OpenGLUtility.h"


const char *vertex_shader_source= 
	"#version 150\n"
	"in vec3 position;\n"
	"out vec2 texture_coordinates;"
	"void main() { texture_coordinates= (position.xy+ vec2(1.0, 1.0))/ 2.0; gl_Position= vec4(position.xy, 0.0, 1.0); }\n";

const char *fragment_shader_source= 
	"#version 150\n"
	"in vec2 texture_coordinates;\n"
	"uniform sampler2D image;\n"
	"void main() { gl_FragColor= texture(image, texture_coordinates.xy); }\n";

const int quad_vertex_count= 3* 6;
float quad_vertices[quad_vertex_count]= { -1.0f, -1.0f, +0.0f, +1.0f, -1.0f, +0.0f, -1.0f, +1.0f, +0.0f, 
					                      +1.0f, -1.0f, +0.0f, +1.0f, +1.0f, +0.0f, -1.0f, +1.0f, +0.0f };

GLuint texture;
SDL_Surface *window_surface;

void GraphicsSystem::Initialize()
{
	screen_width= SCREEN_WIDTH;
	screen_height= SCREEN_HEIGHT;

	SDL_Init(SDL_INIT_VIDEO);

#if NO_OPENGL
	window = SDL_CreateWindow("8-Bit RayTracer", 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									screen_width, 
									screen_height, 
									0);

	window_surface= SDL_GetWindowSurface(window);
	
#else
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	main_window = SDL_CreateWindow("8-Bit RayTracer", 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									screen_width, 
									screen_height, 
									SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(main_window, -1, 0);
	opengl_context = SDL_GL_CreateContext(main_window);
	SDL_GL_SetSwapInterval(0);
	int glew_result= glewInit();
	if(glew_result!= 0)
		cout << "GlewInit() error, returned " << glew_result;
	else
		cout << "OpenGL context: " << glGetString(GL_VERSION);
	cout << "\n\n";
	glDisable(GL_DEPTH_TEST);

	
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, quad_vertex_count* sizeof(float), quad_vertices, GL_STATIC_DRAW);

	GLuint vertex_shader= CreateShader(GL_VERTEX_SHADER, vertex_shader_source, "vertex shader");
	GLuint fragment_shader= CreateShader(GL_FRAGMENT_SHADER, fragment_shader_source, "fragment shader");
	GLuint shader_program= CreateShaderProgram(vertex_shader, fragment_shader);
	glUseProgram(shader_program);

	GLint position_attribute= glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(position_attribute);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, System::graphics.screen_width, System::graphics.screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
#endif
}

void GraphicsSystem::Terminate()
{
	
}

Image GraphicsSystem::GetWindowImage()
{
	SDL_LockSurface(window_surface);
	return Image((Pixel *)window_surface->pixels);
}

void GraphicsSystem::ReturnWindowImage(Image window_image)
{
	SDL_UnlockSurface(window_surface);
}

void GraphicsSystem::Display(Image &image)
{
#if NO_OPENGL
#if !DRAW_DIRECTLY_TO_SCREEN
	Pixel *pixels= (Pixel *)window_surface->pixels;
	for(unsigned int j= 0; j< screen_height; j++)
	{
		for(unsigned int i= 0; i< screen_width; i++)
		{
			int offset= j* screen_width+ i;

			pixels[offset].r= image.pixels[offset].r;
			pixels[offset].g= image.pixels[offset].g;
			pixels[offset].b= image.pixels[offset].b;
		}
	}
#endif

	SDL_UpdateWindowSurface(window);

#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, System::graphics.screen_width, System::graphics.screen_height, GL_RGB, GL_UNSIGNED_BYTE, image.pixels);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count);
	SDL_GL_SwapWindow(System::graphics.main_window);

#endif

	frame_count++;
}

int GraphicsSystem::GetFrameCount()
{
	return frame_count;
}