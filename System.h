#ifndef EBR_SYSTEM
#define EBR_SYSTEM


class SupportSystem;
class EmbreeSystem;
class GraphicsSystem;
class InputSystem;
class ResourceSystem;
class GameSystem;

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
	static InputSystem input;
	static ResourceSystem resource;
	static GameSystem game;

	static void InitializeSystems();//Add flags to initialize only certain systems. 
	static void TerminateSystems();
};

#endif