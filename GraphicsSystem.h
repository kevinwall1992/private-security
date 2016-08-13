#ifndef EBR_GRAPHICS_SYSTEM
#define EBR_GRAPHICS_SYSTEM

#include "GraphicsHeaders.h"

#include "System.h"
#include "Image.h"


class GraphicsSystem : public System
{
	int frame_count= 0;
	
protected:
	void Initialize();
	void Terminate();

public:
	SDL_Window *main_window;
	SDL_Renderer *renderer;
	SDL_GLContext opengl_context;
	int screen_width;
	int screen_height;

	void Display(Image &image);

	int GetFrameCount();

	friend System;
};

#endif