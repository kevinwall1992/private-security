#ifndef EBR_SYSTEM
#define EBR_SYSTEM

class EmbreeSystem;
class GraphicsSystem;
class System
{
	static bool systems_initialized;

protected:
	virtual void Initialize()= 0;
	virtual void Terminate()= 0;

public:
	static EmbreeSystem embree;
	static GraphicsSystem graphics;
	static void InitializeSystems();
	static void TerminateSystems();
};


#include "GraphicsSystem.h"
#include "EmbreeSystem.h"

#endif