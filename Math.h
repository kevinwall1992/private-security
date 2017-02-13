#ifndef EBR_MATH
#define EBR_MATH

#include "Vector.h"
#include "Matrix.h"
#include "Transform.h"

#include <math.h>

//Thanks Microsoft
#ifdef GetObject
#undef GetObject
#endif

//Would like to replace this with namespace math constants
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288
#endif


//Consider namespacing these and Utility.h functions

void SetFloat3(float *destination, const Vec3f &vector);

int IntegerPow(int base, int exp);

int GCD(int u, int v);

float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);

float GetAspectRatio(int width, int height);

#endif