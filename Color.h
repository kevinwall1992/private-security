#ifndef EBR_COLOR
#define EBR_COLOR

#include "Math.h"
#include "GraphicsLibraries.h"

class Color : public Vec4f
{
	using Vec4f::x;
	using Vec4f::y;
	using Vec4f::z;
	using Vec4f::w;

public:
	FloatProperty r, g, b, a;

	Color(float r, float g, float b, float a);
	Color(float r, float g, float b);
	Color(const Color &other);
	Color(const Vec4f &other);
	Color(const Vector<4, float> &other);
	Color();

	using Vec4f::operator=;

	operator Vec3f() const;

	SDL_Color ToSDLColor();

	static Color Black, White, Red, Blue, 
				 Green, Yellow, Purple, Orange, 
				 Pink, Gray, Cornflower;
};

#endif