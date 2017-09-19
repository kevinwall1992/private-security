#ifndef PS_TEXTURE
#define PS_TEXTURE

#include "GraphicsLibraries.h"
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

public:
	Texture(int width, int height, int index= 0);
	Texture(Vec2i size, int index= 0);
	Texture(ColorImage image, int index= 0);
	Texture(const Texture &other);
	Texture();

	Texture & operator=(const Texture &other);

	void SetInterpolationMode(GLenum interpolation_mode);

	virtual void UploadImage(ColorImage image);

	GLuint GetHandle();

	void BindToIndex(GLuint index);

	void Free();

	virtual void Resize(Vec2i size);

	static Texture GetBoundTexture(int index= 0);
};

class DepthTexture: public Texture
{
	void UploadImage(ColorImage image);

public:
	DepthTexture(int width, int height, int index= 0);
	DepthTexture(Vec2i size, int index= 0);
	DepthTexture(DepthImage image, int index= 0);
	DepthTexture(Texture texture);
	DepthTexture(const DepthTexture &other);
	DepthTexture();

	using Texture::operator=;

	void UploadImage(DepthImage image);

	virtual void Resize(Vec2i size);
};

#endif