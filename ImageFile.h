#ifndef PS_IMAGE_FILE
#define PS_IMAGE_FILE

#include "GraphicsLibraries.h"
#include "FileResource.h"
#include "Image.h"


class ImageFile : public FileResource<ImageFile>
{
	SDL_Surface *surface;	

	ImageFile(string filepath);

public:
	Vec2i GetSize();

	ColorImage MakeImage();

	static string MakeFilepath(string filename);
	static vector<ImageFile *> Parse(string filename);
};

#endif
