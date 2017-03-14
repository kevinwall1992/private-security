#ifndef PS_RASTERIZABLE
#define PS_RASTERIZABLE

#include "GraphicsLibraries.h"
#include "VAO.h"


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
	static VAO vao;

	static FullScreenQuad instance;

	FullScreenQuad();

	virtual void Rasterize();

public:
	static void RasterizeInstance();
};

void RasterizeFullScreenQuad();


#endif
