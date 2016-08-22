#ifndef EBR_IMAGE
#define EBR_IMAGE

struct Pixel
{
	unsigned char b, g, r, a;
};

struct Image
{
	Image(int width, int height);
	Image(Pixel *pixels);
	Image();
	~Image();

	void SetPixels(Pixel *pixels);
	Pixel * GetPixels();

	void Resize(int width, int height);

	Pixel & operator[](const int index);

private:
	Pixel *pixels;
	bool pixels_are_owned= true;
};

#endif