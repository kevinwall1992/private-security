#ifndef EBR_GRAPHICS_SYSTEM
#define EBR_GRAPHICS_SYSTEM

#include "GraphicsLibraries.h"

#include "System.h"
#include "Image.h"
#include "Interface.h"


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
	Vec2i GetScreenSize();
	
	void Display(Drawable *drawable);

	int GetFrameCount();

	friend System;
};

#endif