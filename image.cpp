#include "Image.h"

RGBAPixel RGBAPixel::Black(0, 0, 0, 255);
RGBAPixel RGBAPixel::White(255, 255, 255, 255);

RGBAPixel::RGBAPixel(unsigned char r_, unsigned char b_, unsigned char g_, unsigned char a_)
{
	r= r_;
	g= g_;
	b= b_;
	a= a_;
}

RGBAPixel::RGBAPixel()
{
	*this= Black;
}

ColorImage Utility::MakeColorImageFromSDL_Surface(SDL_Surface *surface)
{
	SDL_LockSurface(surface);

	Pixel *image_pixels= new Pixel[surface->w* surface->h];

	switch(surface->format->BitsPerPixel)
	{
	case 24:
		for(int y= 0; y< surface->h; y++)
		{
			for(int x= 0; x< surface->w; x++)
			{
				Pixel *image_pixel= image_pixels+ y* surface->w+ x;
				BGRPixel *surface_pixel= (BGRPixel *)((unsigned char *)surface->pixels+ surface->pitch* y)+ x;

				image_pixel->r= surface_pixel->r;
				image_pixel->g= surface_pixel->g;
				image_pixel->b= surface_pixel->b;
			}
		}

		break;


	case 32:
		if(surface->format->Bmask< surface->format->Rmask)
		{
			for(int y= 0; y< surface->h; y++)
			{
				for(int x= 0; x< surface->w; x++)
				{
					Pixel *image_pixel= image_pixels+ y* surface->w+ x;
					BGRAPixel *surface_pixel= (BGRAPixel *)((unsigned char *)surface->pixels+ surface->pitch* y)+ x;

					image_pixel->r= surface_pixel->r;
					image_pixel->g= surface_pixel->g;
					image_pixel->b= surface_pixel->b;
					image_pixel->a= surface_pixel->a;
				}
			}
		}
		else
		{
			for(int y= 0; y< surface->h; y++)
			{
				for(int x= 0; x< surface->w; x++)
				{
					Pixel *image_pixel= image_pixels+ y* surface->w+ x;
					Pixel *surface_pixel= (Pixel *)((unsigned char *)surface->pixels+ surface->pitch* y)+ x;

					image_pixel->r= surface_pixel->r;
					image_pixel->g= surface_pixel->g;
					image_pixel->b= surface_pixel->b;
					image_pixel->a= surface_pixel->a;
				}
			}
		}

		break;


	default:
		assert(false && "Utility::SDL_SurfaceToImage: Unsupported image type.");
		break;
	}

	SDL_UnlockSurface(surface);

	return ColorImage(surface->w, surface->h, image_pixels);
}

void Utility::SaveColorImageToFile(ColorImage image, string filename)
{
	SDL_Surface *surface= SDL_CreateRGBSurfaceFrom(image.GetPixels(), 
		                                           image.Width, 
												   image.Height, 
												   32, 
												   4* image.Width, 
												   0x000000ff, 
												   0x0000ff00, 
												   0x00ff0000, 
												   0xff000000);

	SDL_SaveBMP(surface, filename.c_str());
}
