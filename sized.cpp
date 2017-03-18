#include "Sized.h"


float Sized::GetAspectRatio()
{
	return Width/ (float)Height;
}

Sized::Sized(Vec2i size_)
	: Size(&size), Width(size.c+ 0), Height(size.c+ 1)
{
	size= size_;
}

Sized::Sized()
	: Size(&size), Width(size.c+ 0), Height(size.c+ 1)
{
	
}


void Resizable::SetSize(Vec2i size_)
{
	size= size_;
}

Resizable::Resizable(Vec2i size_)
	: Sized(size_), Size(&size, this), Width(this, 0), Height(this, 1)
{
	
}

Resizable::Resizable()
	: Sized(), Size(&size, this), Width(this, 0), Height(this, 1)
{
}

void Resizable::Resize(Vec2i size)
{
	if(size== Size)
		return;

	SetSize(size);
}
