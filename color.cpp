#include "Color.h"


Color Color::Black		(0.000f, 0.000f, 0.000f);
Color Color::White		(1.000f, 1.000f, 1.000f);
Color Color::Red		(1.000f, 0.100f, 0.100f);
Color Color::Blue		(0.100f, 0.100f, 1.000f);
Color Color::Green		(0.100f, 1.000f, 0.100f);
Color Color::Yellow		(1.000f, 1.000f, 0.100f);
Color Color::Purple		(0.800f, 0.100f, 1.000f);
Color Color::Orange		(1.000f, 0.600f, 0.100f);
Color Color::Pink		(1.000f, 0.500f, 0.500f);
Color Color::Gray		(0.500f, 0.500f, 0.500f);
Color Color::Cornflower	(0.392f, 0.584f, 0.929f);

Color::Color(float r_, float g_, float b_, float a_)
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	r= r_;
	g= g_;
	b= b_;
	a= a_;
}

Color::Color(float r_, float g_, float b_)
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	r= r_;
	g= g_;
	b= b_;
	a= 1.0f;
}

Color::Color(const Color & other)
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	operator=(other);
}

Color::Color(const Vec4f &other)
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	operator=(other);
}

Color::Color(const Vector<4, float>& other)
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	operator=(other);
}

Color::Color()
	: r(c+ 0), g(c+ 1), b(c+ 2), a(c+ 3)
{
	r= 0.0f;
	g= 0.0f;
	b= 0.0f;
	a= 1.0f;
}

Color::operator Vec3f() const
{
	return Pop();
}

SDL_Color Color::ToSDLColor()
{
	return { (unsigned char)(r* 255), 
			 (unsigned char)(g* 255), 
			 (unsigned char)(b* 255), 
			 255 };
}

Color Color::Mix(Color a, Color b, float a_share)
{
	if(a_share> 1)
		a_share= 1;
	else if(a_share< 0)
		a_share= 0;

	return a* a_share+ b* (1- a_share);
}
