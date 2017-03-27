#ifndef PS_DRAWABLE
#define PS_DRAWABLE

#include "GraphicsLibraries.h"
#include "Vector.h"

#include <stack>


//Intended to higher level, arbitrary OpenGL visualization; 
//Draws directly to currently bound framebuffer

class Drawable
{
	bool is_hidden= false;

public:

	virtual void Show();
	virtual void Hide();
	bool IsHidden();

	virtual void Draw()= 0;
};


#endif
