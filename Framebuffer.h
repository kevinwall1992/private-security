#ifndef PS_FRAMEBUFFER
#define PS_FRAMEBUFFER

#include "GraphicsLibraries.h"
#include "Texture.h"
#include "Common.h"
#include "Vector.h"


class Framebuffer
{
	static Framebuffer default_framebuffer, current_read_framebuffer, current_draw_framebuffer;

	GLuint handle;

	vector<int> active_color_attachment_indices;

	vector<Texture> color_textures;
	DepthTexture depth_texture;

	Framebuffer(bool is_default);

	void UpdateDrawBuffers();

public:
	Framebuffer();

	bool IsDefault();

	GLuint GetHandle();

	Vec2i GetSize();

	void Clear();
	void BindAsReadFramebuffer();
	void BindAsDrawFramebuffer();
	void Bind();
	void ActivateDefaultDrawBuffers();
	void SetViewport();
	void PrepareForDrawing(bool clear= true);

	void SetReadAttachment(int attachment_index);
	void SetDrawAttachment(int attachment_index);
	void SetReadAndDrawAttachment(int attachment_index);
	void BlitOnto(Framebuffer draw_frame_buffer, bool blit_color, bool blit_depth, Vec2i draw_offset= Vec2i(), Vec2i draw_size= Vec2i());

	void AttachColorTexture(Texture texture, int attachment_index);
	void AttachDepthTexture(DepthTexture texture);

	void ActivateColorAttachment(int attachment_index);
	void DeactivateColorAttachment(int attachment_index);

	bool IsComplete();
	void CheckCompleteness();

	void Free();

	static Framebuffer GetDefaultFramebuffer();
	static Framebuffer GetCurrentReadFramebuffer();
	static Framebuffer GetCurrentDrawFramebuffer();
	static Framebuffer GetCurrentFramebuffer();
};

#endif
