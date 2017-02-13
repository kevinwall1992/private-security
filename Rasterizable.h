#ifndef PS_RASTERIZABLE
#define PS_RASTERIZABLE

#include "GraphicsHeaders.h"


class Rasterizable
{
public:
	virtual void Rasterize()= 0;
};

class BakedRasterizable : public Rasterizable
{

public:
	BakedRasterizable();

	virtual void Rasterize();
};


//Assumes position is first attribute
class FullScreenQuad : public Rasterizable
{
	static float vertices[18];
	static GLuint vao_handle;

	static FullScreenQuad instance;

	FullScreenQuad();

	virtual void Rasterize();

public:
	static void RasterizeInstance();
};

void RasterizeFullScreenQuad();


#endif
