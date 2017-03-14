#include "ImageFile.h"

ImageFile::ImageFile(string filepath)
	: FileResource(filepath)
{
	surface= IMG_Load(filepath.c_str());

	switch(surface->format->BytesPerPixel)
	{
	case 3: type= Type::BGR; break;
	case 4: type= Type::RBGA; break;
	default: type= Type::Unknown; break;
	}
}

ImageFile::Type ImageFile::GetType()
{
	return type;
}

Vec2i ImageFile::GetSize()
{
	return Vec2i(surface->w, surface->h);
}

string ImageFile::MakeFilepath(string filename)
{
	return "data/assets/graphical/images/"+ filename;
}

vector<ImageFile*> ImageFile::Parse(string filename)
{
	return Utility::MakeVector(new ImageFile(MakeFilepath(filename)));
}

ColorImage ImageFile::MakeImage()
{
	Pixel *image_pixels= new Pixel[surface->w* surface->h];

	switch(type)
	{
	case Type::BGR:
		for(int y= 0; y< surface->h; y++)
		{
			for(int x= 0; x< surface->w; x++)
			{
				Pixel *image_pixel= image_pixels+ y* surface->w+ x;
				BGRPixel *surface_pixel= (BGRPixel *)surface->pixels+ y* surface->w+ x;

				image_pixel->r= surface_pixel->r;
				image_pixel->g= surface_pixel->g;
				image_pixel->b= surface_pixel->b;
			}
		}

		break;


	case Type::RBGA:
		for(int y= 0; y< surface->h; y++)
		{
			for(int x= 0; x< surface->w; x++)
			{
				Pixel *image_pixel= image_pixels+ y* surface->w+ x;
				Pixel *surface_pixel= (Pixel *)surface->pixels+ y* surface->w+ x;

				image_pixel->r= surface_pixel->r;
				image_pixel->g= surface_pixel->g;
				image_pixel->b= surface_pixel->b;
				image_pixel->a= surface_pixel->a;
			}
		}

		break;


	case Type::Unknown:
	default:
		assert(false && "ImageFile::ToImage: Unsupported image type.");
		break;
	}

	return ColorImage(surface->w, surface->h, image_pixels);
}
