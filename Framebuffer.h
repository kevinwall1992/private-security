#ifndef PS_FRAMEBUFFER
#define PS_FRAMEBUFFER

#include "GraphicsHeaders.h"
#include "Texture.h"
#include "Common.h"

class Framebuffer
{
	static Framebuffer default_;
	static GLuint currently_bound_framebuffer_handle;

	GLuint handle;

	vector<int> active_color_attachment_indices;

	Framebuffer(bool is_default);

	void UpdateDrawBuffers();

public:
	Framebuffer();

	GLuint GetHandle();

	void Bind();

	void ActivateDefaultDrawBuffers();

	void AttachColorTexture(Texture texture, int attachment_index);
	void AttachDepthTexture(DepthTexture texture);

	void ActivateColorAttachment(int attachment_index);
	void DeactivateColorAttachment(int attachment_index);

	bool IsComplete();
	void CheckCompleteness();//naming?

	static Framebuffer GetDefault();
};

#endif
