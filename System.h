#ifndef EBR_SYSTEM
#define EBR_SYSTEM

class SupportSystem;
class EmbreeSystem;
class GraphicsSystem;
class ResourceSystem;
class System
{
	static bool systems_initialized;

protected:
	virtual void Initialize()= 0;
	virtual void Terminate()= 0;

public:
	static SupportSystem support;
	static EmbreeSystem embree;
	static GraphicsSystem graphics;
	static ResourceSystem resource;

	static void InitializeSystems();
	static void TerminateSystems();
};

#include "SupportSystem.h"
#include "GraphicsSystem.h"
#include "EmbreeSystem.h"
#include "ResourceSystem.h"

#endif