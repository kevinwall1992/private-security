#include "Sized.h"


float Sized::GetAspectRatio()
{
	return Width/ (float)Height;
}

void Sized::Resize(int width_, int height_)
{
	width= width_;
	height= height_;
}

Sized::Sized()
	: Width(&width), Height(&height)
{
	
}


void Resizable::Resize(int width, int height)
{
	if(Width== width && Height== height)
		return;

	Free_Sized();

	Sized::Resize(width, height);

	Initialize_Sized();
}
