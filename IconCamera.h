#ifndef PS_ICON_CAMERA
#define PS_ICON_CAMERA

#include "Camera.h"
#include "Framebuffer.h"


class IconCamera : public Camera
{
	Texture color_texture;
	DepthTexture depth_texture;
	Framebuffer framebuffer;
	bool initialized= false;

	void Initialize(Vec2i size);
	void ResizeResizables(Vec2i size);

public:
	IconCamera(float fov, Vec3f position);
	IconCamera(float fov, Vec3f position, Vec3f look_at_position);

	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types);
};

#endif
