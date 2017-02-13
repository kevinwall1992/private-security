#include "Image.h"
#include "Common.h"

Pixel Pixel::black(0, 0, 0, 1);

Pixel::Pixel(unsigned char r_, unsigned char b_, unsigned char g_, unsigned char a_)
{
	r= r_;
	g= g_;
	b= b_;
	a= a_;
}

Pixel::Pixel()
{

}
