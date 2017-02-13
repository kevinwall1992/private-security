#ifndef EBR_EMBREE_SYSTEM
#define EBR_EMBREE_SYSTEM

#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>

#include <xmmintrin.h>
#include <pmmintrin.h>

#include "EBRParameters.h"
#include "Math.h"
#include "System.h"
#include "ISPCKernels.h"


class EmbreeSystem : public System
{
protected:
	void Initialize();
	void Terminate();

public:
	RTCDevice device;

	friend System;
};


struct ISPCLighting : public ispc::Lighting { ISPCLighting(); ~ISPCLighting(); };
typedef ispc::Mesh ISPCMesh;
typedef ispc::PhongMaterial ISPCMaterial;
typedef ispc::VisibilityRayPacket ISPCVisibilityRayPacket;

struct EmbreeVertex
{
	float x, y, z, w;

	EmbreeVertex(float x_, float y_, float z_)
	{
		x= x_; y= y_; z= z_;
	}

	EmbreeVertex()
	{
		x= 0.0f; y= 0.0f; z= 0.0f;
	}
};

struct EmbreeTriangle
{
	int a, b, c;

	EmbreeTriangle(int a_, int b_, int c_)
	{
		a= a_; b= b_; c= c_;
	}

	EmbreeTriangle()
	{
		a= -1; b= -1; c= -1;
	}
};

#endif