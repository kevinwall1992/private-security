#ifndef PS_SHADOW_CAMERA
#define PS_SHADOW_CAMERA

#include "Camera.h"
#include "Framebuffer.h"


class ShadowCamera : public Camera
{
	Framebuffer framebuffer;
	Texture color_texture;
	DepthTexture depth_texture;

	bool initialized;

	void Initialize(int width, int height);
	void ResizeResizables(int width, int height);

public:
	ShadowCamera(float fov_in_degrees, Vec3f position, Vec3f look_at_position);
	ShadowCamera(float fov_in_degrees, Vec3f position);

	virtual Photo TakePhoto(Scene &scene, int width, int height);
};

#endif
