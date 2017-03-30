#ifndef EBR_IMAGE
#define EBR_IMAGE

#include "Sized.h"
#include "GraphicsLibraries.h"
#include "Common.h"


struct RGBAPixel
{
	unsigned char r, g, b, a;

	RGBAPixel(unsigned char r, unsigned char b, unsigned char g, unsigned char a);
	RGBAPixel();

	static RGBAPixel Black;
};
typedef RGBAPixel Pixel;

struct BGRPixel
{
	unsigned char b, g, r;
};

struct BGRAPixel
{
	unsigned char b, g, r, a;
};

template<class T>
class Image : public Resizable
{
	T *pixels= nullptr;

public:

	//Image takes owership of pixels
	Image(int width, int height, T *pixels_= nullptr)
	{
		Vec2i size= Vec2i(width, height);

		if(pixels_!= nullptr)
		{
			SetSize(size);
			pixels= pixels_;
		}
		else
			Resize(size);
	}

	Image(Vec2i size, T *pixels_= nullptr)
	{
		if(pixels_!= nullptr)
		{
			SetSize(size);
			pixels= pixels_;
		}
		else
			Resize(size);
	}

	Image(const Image &other)
	{
		this->operator=(other);
	}

	Image()
	{
		
	}

	Image & operator=(const Image &other)
	{
		SetSize(other.Size);
		
		if(pixels!= nullptr)
			delete pixels;

		pixels= other.pixels;

		return *this;
	}

	void Free()
	{
		if(pixels!= nullptr)
			delete pixels;
	}

	T * GetPixels()
	{
		return pixels;
	}

	void BecomeCopyOf(Image<T> other)
	{
		assert((Width== other.Width && Height== other.Height) && "Image::BecomeCopyOf(): mismatched image sizes");
		memcpy(pixels, other.pixels, sizeof(T)* GetWidth()* GetHeight());
	}

	void Clear(T clear_value)
	{
		for(int i= 0; i< Width* Height; i++)
			pixels[i]= clear_value;
	}

	virtual void Resize(Vec2i size)
	{
		if(size== Size)
			return;
		SetSize(size);

		Free();

		pixels= new T[Width* Height];
	}
};

typedef Image<Pixel> ColorImage;
typedef Image<float> DepthImage;

namespace Utility
{
	ColorImage MakeColorImageFromSDL_Surface(SDL_Surface *surface);
	void SaveColorImageToFile(ColorImage image, string filename);
}

#endif