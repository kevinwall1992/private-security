#include "Image.h"


Image::Image(int width, int height)
{
	pixels= nullptr;
	Resize(width, height);
}

Image::Image()
{
	pixels= nullptr;
}

Image::~Image()
{
	if(pixels!= nullptr)
		delete pixels;
}

void Image::Resize(int width, int height)
{
	if(pixels!= nullptr)
		delete pixels;

	pixels= new Pixel[width* height];
}