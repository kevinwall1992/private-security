#ifndef EBR_EMBREE_SYSTEM
#define EBR_EMBREE_SYSTEM

#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>

#include <xmmintrin.h>
#include <pmmintrin.h>

#include "Math.h"
#include "System.h"


class EmbreeSystem : public System
{
protected:
	void Initialize();
	void Terminate();

public:
	RTCDevice device;

	friend System;
};


#include "Ray.h"

#endif