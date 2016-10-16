#include "Math.h"


void SetFloat3(float *destination, const Vec3f &vector)
{
	destination[0]= vector[0];
	destination[1]= vector[1];
	destination[2]= vector[2];
}

//Need to decide on Make/Construct/Build/etc for these sorts of functions
Vec3f MakeVec3f(const float components[3])
{
	return Vec3f(components[0], components[1], components[2]);
}

Vec2f::Vec2f(float x_, float y_)
{
	x= x_;
	y= y_;
}

Vec2f::Vec2f()
{
	x= 0.0f;
	y= 0.0f;
}

//http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
int IntegerPow(int base, int exp)
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