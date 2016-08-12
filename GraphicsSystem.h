#ifndef EBR_GRAPHICS_SYSTEM
#define EBR_GRAPHICS_SYSTEM

#include "GraphicsHeaders.h"

#include "System.h"
#include "Image.h"


class GraphicsSystem : public System
{
	
protected:
	void Initialize();
	void Terminate();

public:
	SDL_Window *main_window;
	SDL_Renderer *renderer;
	SDL_GLContext opengl_context;
	int screen_width;
	int screen_height;

	void Render(Image &image);

	friend System;
};

#endif