#ifndef PS_RASTER_CAMERA
#define PS_RASTER_CAMERA

#include "Camera.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Threading.h"
#include "Timer.h"
#include "RayCamera.h"
#include "CompositingCamera.h"
#include "ShadowCamera.h"
#include "Cycle.h"

#include <thread>


class RasterCamera : public Camera
{
	Framebuffer gbuffer_framebuffer;
	Texture diffuse_color_buffer;
	Texture glossiness_buffer;
	Texture normal_buffer;
	Texture indirect_light_stencil;
	DepthTexture depth_buffer;

	Framebuffer phong_framebuffer;
	Texture phong_color_buffer;
	DepthTexture phong_depth_buffer;

	Framebuffer downscale_framebuffer;
	Texture downscale_texture;
	Texture downscale_depth_texture;
	float downscale_factor= 0.5f;

	Photo indirect_light_photo;
	Texture indirect_light_texture;
	bool indirect_light_texture_was_modified;

	RayCamera ray_camera;
	std::thread *raytracing_thread= nullptr;
	bool ray_camera_is_invalid= true;

	ShadowCamera shadow_camera;

	Cycle light_cycle;

	bool initialized= false;
	
	
	void Initialize(Vec2i size);
	void ResizeResizables(Vec2i size);

	void GenerateIndirectLightTexture(Scene *scene, Vec2i size);
	void GenerateCompositingBuffers(Scene *scene, Vec2i size);

	Vec2f GetSceneViewWorldSize();
	Vec2i GetSceneViewPixelSize(Vec2i photo_size);
	float GetPixelToWorldRatio(Vec2i photo_size);
	float GetWorldToPixelRatio(Vec2i photo_size);

	Vec2i GetDownscaleSize(Vec2i size);

public:
	RasterCamera(float fov, Vec3f position);
	RasterCamera(float fov, Vec3f focus, float pitch, float yaw);
	~RasterCamera();

	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types);
};

#endif