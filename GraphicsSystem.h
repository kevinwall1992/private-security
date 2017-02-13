#ifndef EBR_GRAPHICS_SYSTEM
#define EBR_GRAPHICS_SYSTEM

#include "GraphicsHeaders.h"

#include "System.h"
#include "Image.h"


class Photo;
class ShaderProgram;

class GraphicsSystem : public System
{
	SDL_GLContext opengl_context;
	SDL_Window *window;

	int screen_width= 1024;
	int screen_height= 1024;

	int frame_count= 0;

protected:
	void Initialize();
	void Terminate();

public:
	int GetScreenWidth();
	int GetScreenHeight();
	

	void Display(Photo photo);

	int GetFrameCount();

	friend System;
};

#endif