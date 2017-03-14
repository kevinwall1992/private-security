#include "Image.h"

RGBAPixel RGBAPixel::black(0, 0, 0, 1);

RGBAPixel::RGBAPixel(unsigned char r_, unsigned char b_, unsigned char g_, unsigned char a_)
{
	r= r_;
	g= g_;
	b= b_;
	a= a_;
}

RGBAPixel::RGBAPixel()
{

}
