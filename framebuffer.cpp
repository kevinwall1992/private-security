#include "Framebuffer.h"
#include "Common.h"
#include "GraphicsSystem.h"
#include "Viewport.h"

Framebuffer Framebuffer::default_framebuffer(true);
Framebuffer Framebuffer::current_read_framebuffer= Framebuffer::default_framebuffer;
Framebuffer Framebuffer::current_draw_framebuffer= Framebuffer::default_framebuffer;

Framebuffer::Framebuffer(bool is_default)
{
	handle= 0;

	if(!is_default)
		handle= -1;
}

void Framebuffer::UpdateDrawBuffers()
{
	vector<unsigned int> draw_buffers;
	for(unsigned int i= 0; i< active_color_attachment_indices.size(); i++)
		draw_buffers.push_back(GL_COLOR_ATTACHMENT0+ active_color_attachment_indices[i]);

	glDrawBuffers((GLsizei)(draw_buffers.size()), &draw_buffers[0]);

	Utility::HandleOpenGLErrors();
}

Framebuffer::Framebuffer()
{
	handle= -1;
}

bool Framebuffer::IsDefault()
{
	return handle== 0;
}

void Framebuffer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Framebuffer::BindAsReadFramebuffer()
{
	if(handle== -1)
		glGenFramebuffers(1, &handle);

	if(current_read_framebuffer.GetHandle()== handle)
		return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, handle);
	current_read_framebuffer= *this;

	Utility::HandleOpenGLErrors();
}

void Framebuffer::BindAsDrawFramebuffer()
{
	if(handle== -1)
		glGenFramebuffers(1, &handle);

	if(current_draw_framebuffer.GetHandle()== handle)
		return;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handle);
	current_draw_framebuffer= *this;

	Utility::HandleOpenGLErrors();
}

void Framebuffer::Bind()
{
	BindAsDrawFramebuffer();
	BindAsReadFramebuffer();
}

void Framebuffer::ActivateDefaultDrawBuffers()
{
	UpdateDrawBuffers();
}

void Framebuffer::SetViewport()
{
	if(IsDefault())
		Viewport::Set(0, 0, System::graphics.GetScreenSize().x, System::graphics.GetScreenSize().y);
	else
		Viewport::Set(0, 0, color_textures[0].Width, color_textures[0].Height);
}

void Framebuffer::PrepareForDrawing(bool clear)
{
	Bind();
	SetViewport();

	if(clear)
		Clear();

	Utility::HandleOpenGLErrors();
}

void Framebuffer::SetReadAttachment(int attachment_index)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0+ attachment_index);
}

void Framebuffer::SetDrawAttachment(int attachment_index)
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0+ attachment_index);
}

void Framebuffer::SetReadAndDrawAttachment(int attachment_index)
{
	SetReadAttachment(attachment_index);
	SetDrawAttachment(attachment_index);
}

void Framebuffer::BlitOnto(Framebuffer draw_frame_buffer, bool blit_color, bool blit_depth, Vec2i draw_offset, Vec2i draw_size)
{
	if(draw_size.x== 0)
		draw_size= draw_frame_buffer.GetSize();
	Vec2i read_size= GetSize();

	BindAsReadFramebuffer();
	draw_frame_buffer.BindAsDrawFramebuffer();

	GLbitfield mask= (blit_color ? GL_COLOR_BUFFER_BIT : 0) | (blit_depth ? GL_DEPTH_BUFFER_BIT : 0);
	glBlitFramebuffer(0, 0, read_size.x, read_size.y, draw_offset.x, draw_offset.y, draw_offset.x+ draw_size.x, draw_offset.y+ draw_size.y, mask, GL_NEAREST);
}

void Framebuffer::AttachColorTexture(Texture texture, int attachment_index)
{
	Bind();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+ attachment_index, texture.GetHandle(), 0);
	color_textures.push_back(texture);

	Utility::HandleOpenGLErrors();

	ActivateColorAttachment(attachment_index);
}

void Framebuffer::AttachDepthTexture(DepthTexture texture)
{
	Bind();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.GetHandle(), 0);
	depth_texture= texture;

	Utility::HandleOpenGLErrors();
}

void Framebuffer::ActivateColorAttachment(int attachment_index)
{
	Bind();

	bool already_contained= false;
	for(unsigned int i= 0; i< active_color_attachment_indices.size(); i++)
		if(attachment_index== active_color_attachment_indices[i])
			already_contained= true;

	if(!already_contained)
	{
		active_color_attachment_indices.push_back(attachment_index);
		UpdateDrawBuffers();
	}
}

void Framebuffer::DeactivateColorAttachment(int attachment_index)
{
	Bind();

	for(unsigned int i= 0; i< active_color_attachment_indices.size(); i++)
	{
		if(attachment_index== active_color_attachment_indices[i])
		{
			active_color_attachment_indices.erase(active_color_attachment_indices.begin()+ i);
			UpdateDrawBuffers();
			return;
		}
	}
}

bool Framebuffer::IsComplete()
{
	GLenum framebuffer_status= glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(framebuffer_status!= GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "Framebuffer error: " << framebuffer_status;
		return false;
	}

	return true;
}

void Framebuffer::CheckCompleteness()
{
	assert(IsComplete() && "Framebuffer is not complete!");
}

void Framebuffer::Free()
{
	glDeleteFramebuffers(1, &handle);
}

GLuint Framebuffer::GetHandle()
{
	return handle;
}

Vec2i Framebuffer::GetSize()
{
	if(IsDefault())
		return System::graphics.GetScreenSize();
	else if(depth_texture.GetHandle()!= -1)
		return depth_texture.Size;
	else
		return color_textures[0].Size;
}

Framebuffer Framebuffer::GetDefaultFramebuffer()
{
	return default_framebuffer;
}

Framebuffer Framebuffer::GetCurrentReadFramebuffer()
{
	return current_read_framebuffer;
}

Framebuffer Framebuffer::GetCurrentDrawFramebuffer()
{
	return current_draw_framebuffer;
}

Framebuffer Framebuffer::GetCurrentFramebuffer()
{
	return GetCurrentDrawFramebuffer();
}
