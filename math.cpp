#include "Math.h"

namespace Math
{
	void SetFloat3(float *destination, const Vec3f &vector)
	{
		destination[0]= vector[0];
		destination[1]= vector[1];
		destination[2]= vector[2];
	}

	//http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
	int Pow(int base, int exp)
	{
		int result = 1;

		while(exp)
		{
			if(exp& 1)
				result*= base;

			exp>>= 1;
			base*= base;
		}

		return result;
	}

	//http://stackoverflow.com/questions/10956543/gcd-function-in-c-sans-cmath-library
	int GCD(int u, int v) 
	{
		while(v != 0) 
		{
			unsigned r= u% v;

			u= v;
			v= r;
		}

		return u;
	}

	float DegreesToRadians(float degrees)
	{
		return (float)((degrees/ 360)* 2* M_PI);
	}

	float RadiansToDegrees(float radians)
	{
		return (float)((radians/ (2* M_PI))* 360);
	}

	float GetAspectRatio(int width, int height)
	{
		return GetAspectRatio((float)width, (float)height);
	}

	float GetAspectRatio(Vec2i size)
	{
		return GetAspectRatio(size.x, size.y);
	}
	float GetAspectRatio(float width, float height)
	{
		return width/ height;
	}
	float GetAspectRatio(Vec2f size)
	{
		return GetAspectRatio(size.x, size.y);
	}
}
