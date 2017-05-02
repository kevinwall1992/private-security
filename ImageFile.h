#ifndef PS_IMAGE_FILE
#define PS_IMAGE_FILE

#include "GraphicsLibraries.h"
#include "FileResource.h"
#include "Image.h"
#include "Texture.h"


class ImageFile : public FileResource<ImageFile>
{
	SDL_Surface *surface;	

	Texture texture;

	ImageFile(string filepath);

public:
	Vec2i GetSize();

	ColorImage MakeImage();

	//Texture is owned by this ImageFile
	//Probably going to extend this idea to ColorImage in the future
	Texture RetrieveTexture();

	static string MakeFilepath(string filename);
	static vector<ImageFile *> Parse(string filename);
};

#endif
