#ifndef PS_GRAYSCALE_MASK_GENERATOR
#define PS_GRAYSCALE_MASK_GENERATOR

#include "Framebuffer.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"


class GrayscaleMaskGenerator
{
	Framebuffer framebuffer;

	Texture source, destination;

public:
	GrayscaleMaskGenerator(Vec2i size);

	void SetSourceTexture(Texture texture);
	void SetDestinationTexture(Texture texture);

	Texture GetDestinationTexture();

	//These return the destination texture as a nicety, they do not return new textures
	Texture GenerateGrayscaleMask_Quad(Transform transform);
	Texture GenerateGrayscaleMask_Mesh(Mesh *mesh, Transform transform);
};

#endif
