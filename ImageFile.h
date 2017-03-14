#ifndef PS_IMAGE_FILE
#define PS_IMAGE_FILE

#include "GraphicsLibraries.h"
#include "FileResource.h"
#include "Image.h"


class ImageFile : public FileResource<ImageFile>
{
public:
	enum Type { Unknown, BGR, RBGA };

private:
	SDL_Surface *surface;	
	Type type;

	ImageFile(string filepath);

public:
	Type GetType();
	Vec2i GetSize();

	ColorImage MakeImage();

	static string MakeFilepath(string filename);
	static vector<ImageFile *> Parse(string filename);
};

#endif
