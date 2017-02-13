#ifndef PS_TEXTURE
#define PS_TEXTURE

#include "GraphicsHeaders.h"
#include "Image.h"

class TextureBase
{
	virtual GLuint GetHandle()= 0;

	virtual void BindToIndex(GLuint index)= 0;

	virtual void Free()= 0;
};

class Texture : public TextureBase, public Resizable
{
protected:
	GLuint handle;

	virtual void Initialize_Sized();
	virtual void Free_Sized();

public:
	Texture(int width, int height, int index= 0);
	Texture(ColorImage image, int index= 0);
	Texture();

	Texture & operator=(const Texture &other);

	void SetInterpolationMode(GLenum interpolation_mode);

	virtual void UploadImage(ColorImage image);

	GLuint GetHandle();

	void BindToIndex(GLuint index);

	void Free();
};

class DepthTexture: public Texture
{
	void UploadImage(ColorImage image);

public:
	DepthTexture(int width, int height, int index= 0);
	DepthTexture(DepthImage image, int index= 0);
	DepthTexture();

	void UploadImage(DepthImage image);
};

#endif