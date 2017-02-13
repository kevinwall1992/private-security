#ifndef EBR_IMAGE
#define EBR_IMAGE

#include "Sized.h"


struct Pixel
{
	unsigned char b, g, r, a;

	Pixel(unsigned char r, unsigned char b, unsigned char g, unsigned char a);
	Pixel();

	static Pixel black;
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

	Image(int width, int height)
	{
		Resize(width, height);
	}

	Image()
	{
		
	}

	Image & operator=(const Image &other)
	{
		Sized::Resize(other.Width, other.Height);
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

	//This seems to imply a general copy operation,
	//When in fact, its just a blit (that requires size compatibility).
	//Would be nice to come up with different name,
	//But the direction of blit has to be clear.
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