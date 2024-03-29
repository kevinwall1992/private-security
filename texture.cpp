#include "Texture.h"
#include "Common.h"


GLuint CreateTexture(int width, int height, GLenum interpolation_mode, bool is_depth, void *pixels, int index)
{
	GLuint texture_handle;

	glActiveTexture(GL_TEXTURE0+ index);
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	if(is_depth)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation_mode);

	Utility::HandleOpenGLErrors();

	return texture_handle;
}

Texture::Texture(int width, int height, int index)
{
	SetSize(Vec2i(width, height));

	handle= CreateTexture(Width, Height, GL_LINEAR, false, nullptr, index);
}

Texture::Texture(Vec2i size, int index)
{
	SetSize(size);

	handle= CreateTexture(Width, Height, GL_LINEAR, false, nullptr, index);
}

Texture::Texture(ColorImage image, int index)
{
	SetSize(image.Size);

	handle= CreateTexture(Width, Height, GL_LINEAR, false, image.GetPixels(), index);
}

Texture::Texture(const Texture &other)
{
	this->operator=(other);
}

Texture::Texture()
{
	handle= -1;
}

Texture & Texture::operator=(const Texture &other)
{
	SetSize(other.Size);

	handle= other.handle;

	return *this;
}

void Texture::UploadImage(ColorImage image)
{
	BindToIndex(0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.Width, image.Height, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());

	Utility::HandleOpenGLErrors();
}

GLuint Texture::GetHandle()
{
	return handle;
}

void Texture::BindToIndex(GLuint index)
{
	glActiveTexture(GL_TEXTURE0+ index);
	glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture::Free()
{
	glDeleteTextures(1, &handle);
	handle= -1;
}

void Texture::Resize(Vec2i size)
{
	if(handle== -1)
		*this= Texture(size);
	else
	{
		if(size== Size)
			return;
		SetSize(size);

		BindToIndex(0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
}

Texture Texture::GetBoundTexture(int index)
{
	glActiveTexture(GL_TEXTURE0+ index);

	GLint handle;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &handle);

	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	Texture texture;
	texture.handle= handle;
	texture.SetSize(Vec2i(width, height));

	return texture;
}

void DepthTexture::UploadImage(ColorImage image)
{
	assert(false && "DepthTexture::UploadImage(Image) not supported");
}

DepthTexture::DepthTexture(int width, int height, int index)
{
	SetSize(Vec2i(width, height));

	handle= CreateTexture(Width, Height, GL_LINEAR, true, 0, index);
}

DepthTexture::DepthTexture(Vec2i size, int index)
{
	SetSize(size);

	handle= CreateTexture(Width, Height, GL_LINEAR, true, 0, index);
}

DepthTexture::DepthTexture(DepthImage image, int index)
{
	SetSize(image.Size);

	handle= CreateTexture(Width, Height, GL_LINEAR, true, image.GetPixels(), index);
}

DepthTexture::DepthTexture(Texture texture)
{
	SetSize(texture.Size);

	handle= texture.GetHandle();
}

DepthTexture::DepthTexture(const DepthTexture &other)
{
	this->operator=(other);
}

DepthTexture::DepthTexture()
{
	handle= -1;
}

void DepthTexture::UploadImage(DepthImage image)
{
	BindToIndex(0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_DEPTH_COMPONENT, GL_FLOAT, image.GetPixels());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GetWidth(), GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, image);

	Utility::HandleOpenGLErrors();
}

void DepthTexture::Resize(Vec2i size)
{
	if(handle== -1)
		*this= DepthTexture(size);
	else
	{
		if(size== Size)
			return;
		SetSize(size);

		BindToIndex(0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
}
