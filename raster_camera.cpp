#include "RasterCamera.h"
#include "Shader.h"
#include "RayCamera.h"
#include "CompositingCamera.h"
#include "ShadowCamera.h"


void RasterCamera::Initialize(int width, int height)
{
	if(initialized)
		return;

	color_buffer= Texture(width, height, 1);
	glossiness_buffer= Texture(width, height, 2);
	normal_buffer= Texture(width, height, 3);
	depth_buffer= DepthTexture(width, height, 4);
	
	gbuffer_framebuffer.Bind();
	gbuffer_framebuffer.AttachColorTexture(color_buffer, 0);
	gbuffer_framebuffer.AttachColorTexture(glossiness_buffer, 1);
	gbuffer_framebuffer.AttachColorTexture(normal_buffer, 2);
	gbuffer_framebuffer.AttachDepthTexture(depth_buffer);
	gbuffer_framebuffer.CheckCompleteness();

	compositing_color_buffer= Texture(width, height);
	compositing_glossiness_buffer= Texture(width, height);
	compositing_normal_buffer= Texture(width, height);
	compositing_depth_buffer= DepthTexture(width, height);

	compositing_framebuffer.Bind();
	compositing_framebuffer.AttachColorTexture(compositing_color_buffer, 0);
	compositing_framebuffer.AttachColorTexture(compositing_glossiness_buffer, 1);
	compositing_framebuffer.AttachColorTexture(compositing_normal_buffer, 2);
	compositing_framebuffer.AttachDepthTexture(compositing_depth_buffer);
	compositing_framebuffer.CheckCompleteness();


	phong_framebuffer.Bind();

	phong_color_buffer= Texture(width, height);
	phong_framebuffer.AttachColorTexture(phong_color_buffer, 0);

	phong_depth_buffer= DepthTexture(width, height);
	phong_framebuffer.AttachDepthTexture(phong_depth_buffer);

	phong_framebuffer.CheckCompleteness();


	ShaderProgram *shader_program= ShaderProgram::Retrieve("phong.program");
	shader_program->Use();
	shader_program->SetAttribute("position", 3, sizeof(float)* 3, 0);
	shader_program->SetUniformInt("diffuse_buffer", 1);
	shader_program->SetUniformInt("glossiness_buffer", 2);
	shader_program->SetUniformInt("normal_buffer", 3);
	shader_program->SetUniformInt("depth_buffer", 4);
	shader_program->SetUniformInt("indirect_buffer", 5);
	shader_program->SetUniformInt("shadow_map", 6);
	shader_program->SetUniformMatrix4x4f("projection_transform", Transform::MakeProjectionTransform(FOV, GetAspectRatio(width, height)));

	
	indirect_light_texture= Texture(width, height, 5);
	indirect_light_texture_was_modified= false;


	initialized= true;
}

void RasterCamera::ResizeResizables(int width, int height)
{
	color_buffer.Resize(width, height);
	glossiness_buffer.Resize(width, height);
	normal_buffer.Resize(width, height);
	depth_buffer.Resize(width, height);
	
	compositing_color_buffer.Resize(width, height);
	compositing_glossiness_buffer.Resize(width, height);
	compositing_normal_buffer.Resize(width, height);
	compositing_depth_buffer.Resize(width, height);

	phong_color_buffer.Resize(width, height);
	phong_depth_buffer.Resize(width, height);
	
	indirect_light_texture.Resize(width, height);
}

void RasterCamera::Update()
{
	ray_camera_is_invalid= true;
	compositing_camera_is_invalid= true;
}

void RasterCamera::GenerateIndirectLightTexture(Scene *scene, int width, int height)
{
	if(ray_camera_is_invalid)
	{
		ray_camera.AssumeOrientation(*this);
		ray_camera_is_invalid= false;
	}

	indirect_light_photo= ray_camera.TakePhoto(*scene, width, height);
	indirect_light_texture_was_modified= true;
}

void RasterCamera::GenerateCompositingBuffers(Scene *scene, int width, int height)
{
	if(compositing_camera_is_invalid)
	{
		compositing_camera.AssumeOrientation(*this);
		//compositing_camera_is_invalid= false;
	}

	compositing_camera.TakePhoto(*scene, width, height);

	compositing_buffers_were_modified= true;

}

RasterCamera::RasterCamera(float fov, Vec3f position)
	: Camera(fov, position), ray_camera(fov, position), compositing_camera(fov, position), shadow_camera(DegreesToRadians(110), Vec3f())
{
	animation_timer.Start();
}

RasterCamera::~RasterCamera()
{
	raytracing_thread->join();
	delete raytracing_thread;

	if(compositing_thread!= nullptr)
	{
		compositing_thread->join();
		delete compositing_thread;
	}
}

Photo RasterCamera::TakePhoto(Scene &scene, int width, int height)
{
	Initialize(width, height);
	ResizeResizables(width, height);
	CatchUp();


	animation_timer.Pause();
	float elapsed_seconds= animation_timer.GetElapsedSeconds();
	animation_timer.Start();


	if(indirect_light_texture_was_modified || raytracing_thread== nullptr)
	{
		if(indirect_light_texture_was_modified)
		{
			raytracing_thread->join();
			delete raytracing_thread;

			indirect_light_texture.UploadImage(indirect_light_photo.GetImage());
			indirect_light_texture_was_modified= false;
		}

		raytracing_thread= new std::thread(&RasterCamera::GenerateIndirectLightTexture, this, &scene, width, height);
	}

	if(compositing_buffers_were_modified || compositing_thread== nullptr)
	{
		if(compositing_buffers_were_modified)
		{
			if(compositing_thread!= nullptr)
			{
				compositing_thread->join();
				delete compositing_thread;
				compositing_thread= nullptr;
			}

			compositing_color_buffer.UploadImage(compositing_camera.GetDiffuseMap());
			compositing_glossiness_buffer.UploadImage(compositing_camera.GetGlossinessMap());
			compositing_normal_buffer.UploadImage(compositing_camera.GetNormalMap());
			compositing_depth_buffer.UploadImage(compositing_camera.GetDepthMap());

			compositing_buffers_were_modified= false;
		}

		if(compositing_camera_is_invalid)
		{
			compositing_thread= new std::thread(&RasterCamera::GenerateCompositingBuffers, this, &scene, width, height);

			compositing_camera_is_invalid= false;
		}
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, compositing_framebuffer.GetHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer_framebuffer.GetHandle());

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	gbuffer_framebuffer.ActivateDefaultDrawBuffers();
	glViewport(0, 0, width, height);

	ShaderProgram *gbuffer_shader_program= ShaderProgram::Retrieve("gbuffer.program");
	gbuffer_shader_program->Use();
	gbuffer_shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(width, height).GetMatrix());

	scene.RasterizeConditionally(Prop::DrawFlags::RasterizeGbuffers);


	Light *light= scene.GetLights()[0];
	Vec3f light_position= light->GetPosition();
	Vec3f raster_light_position= Transform().Translate(Vec3f(7, 0, 0)).RotateAboutY((float)(elapsed_seconds* M_PI/ 3)).Apply(Vec3f())+ light->GetPosition();
	shadow_camera.Position= raster_light_position;
	shadow_camera.LookAt(Vec3f(28, -5, -20));
	shadow_camera.TakePhoto(scene, 2048, 2048).GetTexture().BindToIndex(6);


	phong_framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	ShaderProgram *phong_shader_program= ShaderProgram::Retrieve("phong.program");
	phong_shader_program->Use();
	phong_shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(width, height).GetMatrix());
	phong_shader_program->SetUniformVector3f("camera_position", Position);
	phong_shader_program->SetUniformVector3f("light_position", raster_light_position);
	phong_shader_program->SetUniformVector3f("light_intensity", light->GetIntensity());
	phong_shader_program->SetUniformMatrix4x4f("shadow_camera_transform", shadow_camera.GetProjectedTransform(2048, 2048).GetMatrix());

	RasterizeFullScreenQuad();

	return Photo(phong_color_buffer);
}
