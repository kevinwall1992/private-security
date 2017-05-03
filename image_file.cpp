#include "ImageFile.h"


ImageFile::ImageFile(string filepath)
	: FileResource(filepath)
{
	IMG_Init(IMG_INIT_PNG);

	surface= IMG_Load(filepath.c_str());
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
	return Utility::MakeColorImageFromSDL_Surface(surface);
}

ColorImage ImageFile::RetrieveImage()
{
	if(image.GetPixels()== nullptr)
		image= MakeImage();

	return image;
}

Texture ImageFile::RetrieveTexture()
{
	if(texture.GetHandle()== -1)
		texture= Texture(RetrieveImage());

	return texture;
}
