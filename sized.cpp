#include "Sized.h"


Vec2i Sized::GetSize() const
{
	return Vec2i(Width, Height);
}

float Sized::GetAspectRatio()
{
	return Width/ (float)Height;
}

void Sized::Resize(Vec2i size_)
{
	size= size_;
}

Sized::Sized()
	: Width(size.c+ 0), Height(size.c+ 1)
{
	
}


void Resizable::Resize(Vec2i size)
{
	if(Width== size.x && Height== size.y)
		return;

	Free_Sized();

	Sized::Resize(size);

	Initialize_Sized();
}
