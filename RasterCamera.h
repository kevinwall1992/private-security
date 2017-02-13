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

#include <thread>

class RasterCamera : public Camera
{
	Framebuffer gbuffer_framebuffer, compositing_framebuffer;
	Texture color_buffer, compositing_color_buffer;
	Texture glossiness_buffer, compositing_glossiness_buffer;
	Texture normal_buffer, compositing_normal_buffer;
	DepthTexture depth_buffer, compositing_depth_buffer;

	Framebuffer phong_framebuffer;
	Texture phong_color_buffer;
	DepthTexture phong_depth_buffer;

	Photo indirect_light_photo;
	Texture indirect_light_texture;
	bool indirect_light_texture_was_modified;

	CompositingCamera compositing_camera;
	std::thread *compositing_thread= nullptr;
	bool compositing_camera_is_invalid= true;
	bool compositing_buffers_were_modified;

	RayCamera ray_camera;
	std::thread *raytracing_thread= nullptr;
	bool ray_camera_is_invalid= true;

	ShadowCamera shadow_camera;

	Timer animation_timer;

	bool initialized= false;
	
	
	void Initialize(int width, int height);
	void ResizeResizables(int width, int height);
	void Update();

	void GenerateIndirectLightTexture(Scene *scene, int width, int height);
	void GenerateCompositingBuffers(Scene *scene, int width, int height);

public:
	RasterCamera(float fov, Vec3f position);
	~RasterCamera();

	virtual Photo TakePhoto(Scene &scene, int width, int height);
};

#endif