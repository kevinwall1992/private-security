#include "Image.h"
#include "Common.h"


Image::Image(int width, int height)
{
	pixels= nullptr;
	Resize(width, height);
}

Image::Image(Pixel *pixels_)
{
	pixels= pixels_;
	pixels_are_owned= false;
}

Image::Image()
{
	pixels= nullptr;
}

Image::~Image()
{
	if(pixels!= nullptr && pixels_are_owned)
		delete pixels;
}

void Image::SetPixels(Pixel *pixels_)
{
	if(pixels!= nullptr && pixels_are_owned)
		delete pixels;

	pixels= pixels_;
	pixels_are_owned= false;
}

Pixel * Image::GetPixels()
{
	return pixels;
}

void Image::Resize(int width, int height)
{
	assert(pixels_are_owned && "Tried to resize image that doesn't own its pixels");

	if(pixels!= nullptr)
		delete pixels;

	pixels= new Pixel[width* height];
}