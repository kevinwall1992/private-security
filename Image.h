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

	static RGBAPixel black;
};
typedef RGBAPixel Pixel;

struct BGRPixel
{
	unsigned char b, g, r;
};

template<class T>
class Image : public Resizable
{
	T *pixels= nullptr;

protected:
	void Initialize_Sized()
	{
		pixels= new T[Width* Height];
	}

	void Free_Sized()
	{
		if(pixels!= nullptr)
			delete pixels;
	}

public:

	//Image takes owership of pixels
	Image(int width, int height, T *pixels_= nullptr)
	{
		Vec2i size= Vec2i(width, height);

		if(pixels_!= nullptr)
		{
			Sized::Resize(size);
			pixels= pixels_;
		}
		else
			Resize(size);
	}

	Image(Vec2i size, T *pixels_= nullptr)
	{
		if(pixels_!= nullptr)
		{
			Sized::Resize(size);
			pixels= pixels_;
		}
		else
			Resize(size);
	}

	Image()
	{
		
	}

	Image & operator=(const Image &other)
	{
		Sized::Resize(other.GetSize());
		
		pixels= other.pixels;

		return *this;
	}

	void Free()
	{
		Free_Sized();
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
};

typedef Image<Pixel> ColorImage;
typedef Image<float> DepthImage;

#endif