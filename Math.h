#ifndef EBR_MATH
#define EBR_MATH

#include "Vector.h"
#include "Ray.h"
#include "Matrix.h"
#include "Transform.h"

#include <math.h>
#include "MicrosoftPollutionCleaner.h"

namespace Math
{
	#ifndef M_PI
	#define M_PI    3.14159265358979323846264338327950288
	#endif

	void SetFloat3(float *destination, const Vec3f &vector);

	int Pow(int base, int exp);

	int GCD(int u, int v);

	float DegreesToRadians(float degrees);
	float RadiansToDegrees(float radians);

	float GetAspectRatio(float width, float height);
	float GetAspectRatio(Vec2f size);
}

#endif