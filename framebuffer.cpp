#include "Framebuffer.h"
#include "Common.h"
#include "OpenGLUtility.h"

GLuint Framebuffer::currently_bound_framebuffer_handle= 0;
Framebuffer Framebuffer::default_(true);

Framebuffer::Framebuffer(bool is_default)
{
	if(!is_default)
		assert("false" && "Private Framebuffer constructor only for default framebuffer");

	handle= 0;
}

void Framebuffer::UpdateDrawBuffers()
{
	vector<unsigned int> draw_buffers;
	for(unsigned int i= 0; i< active_color_attachment_indices.size(); i++)
		draw_buffers.push_back(GL_COLOR_ATTACHMENT0+ active_color_attachment_indices[i]);

	glDrawBuffers((GLsizei)(draw_buffers.size()), &draw_buffers[0]);

	HandleGLErrors();
}

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &handle);
}

void Framebuffer::Bind()
{
	if(currently_bound_framebuffer_handle== handle)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, handle);
	currently_bound_framebuffer_handle= handle;

	HandleGLErrors();
}

void Framebuffer::ActivateDefaultDrawBuffers()
{
	UpdateDrawBuffers();
}

void Framebuffer::AttachColorTexture(Texture texture, int attachment_index)
{
	Bind();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+ attachment_index, texture.GetHandle(), 0);

	HandleGLErrors();

	ActivateColorAttachment(attachment_index);
}

void Framebuffer::AttachDepthTexture(DepthTexture texture)
{
	Bind();

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.GetHandle(), 0);

	HandleGLErrors();
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

GLuint Framebuffer::GetHandle()
{
	return handle;
}

Framebuffer Framebuffer::GetDefault()
{
	return default_;
}
