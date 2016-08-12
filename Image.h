#ifndef EBR_IMAGE
#define EBR_IMAGE

struct Pixel
{
	unsigned char r, g, b;
};

struct Image
{
	Pixel *pixels;

	Image(int width, int height);
	Image();
	~Image();

	void Resize(int width, int height);
};

#endif