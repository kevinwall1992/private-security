#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

void HandleOpenGLErrors();

//Thanks Microsoft
#ifdef GetObject
#undef GetObject
#endif