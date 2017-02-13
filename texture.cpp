#include "Texture.h"
#include "Common.h"
#include "OpenGLUtility.h"


GLuint CreateTexture(int width, int height, GLenum interpolation_mode, bool is_depth, void *pixels, int index)
{
	GLuint texture_handle;

	glActiveTexture(GL_TEXTURE0+ index);
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	if(is_depth)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation_mode);

	HandleGLErrors();

	return texture_handle;
}

void Texture::Initialize_Sized()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
}

void Texture::Free_Sized()
{
	glDeleteTextures(1, &handle);
}

Texture::Texture(int width, int height, int index)
{
	Sized::Resize(width, height);

	handle= CreateTexture(Width, Height, GL_LINEAR, false, nullptr, index);
}

Texture::Texture(ColorImage image, int index)
{
	Sized::Resize(image.Width, image.Height);

	handle= CreateTexture(Width, Height, GL_LINEAR, false, image.GetPixels(), index);
}

Texture::Texture()
{

}

Texture & Texture::operator=(const Texture &other)
{
	Sized::Resize(other.Width, other.Height);

	handle= other.handle;

	return *this;
}

void Texture::UploadImage(ColorImage image)
{
	BindToIndex(0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.Width, image.Height, GL_BGRA, GL_UNSIGNED_BYTE, image.GetPixels());

	HandleGLErrors();
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
}

void DepthTexture::UploadImage(ColorImage image)
{
	assert(false && "DepthTexture::UploadImage(Image) not supported");
}

DepthTexture::DepthTexture(int width, int height, int index)
{
	Sized::Resize(width, height);

	handle= CreateTexture(Width, Height, GL_LINEAR, true, 0, index);
}

DepthTexture::DepthTexture(DepthImage image, int index)
{
	Sized::Resize(image.Width, image.Height);

	handle= CreateTexture(Width, Height, GL_LINEAR, true, image.GetPixels(), index);
}

DepthTexture::DepthTexture()
{

}

void DepthTexture::UploadImage(DepthImage image)
{
	BindToIndex(0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_DEPTH_COMPONENT, GL_FLOAT, image.GetPixels());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GetWidth(), GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, image);

	HandleGLErrors();
}