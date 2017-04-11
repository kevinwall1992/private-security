#include "RasterCamera.h"
#include "Shader.h"
#include "RayCamera.h"
#include "CompositingCamera.h"
#include "ShadowCamera.h"
#include "Viewport.h"


//#define NO_RAYTRACING

void RasterCamera::Initialize(Vec2i size)
{
	if(initialized)
		return;

	diffuse_color_buffer= Texture(size, 1);
	glossiness_buffer= Texture(size, 2);
	normal_buffer= Texture(size, 3);
	depth_buffer= DepthTexture(size, 4);
	
	gbuffer_framebuffer.Bind();
	gbuffer_framebuffer.AttachColorTexture(diffuse_color_buffer, 0);
	gbuffer_framebuffer.AttachColorTexture(glossiness_buffer, 1);
	gbuffer_framebuffer.AttachColorTexture(normal_buffer, 2);
	gbuffer_framebuffer.AttachDepthTexture(depth_buffer);
	gbuffer_framebuffer.CheckCompleteness();

	compositing_diffuse_color_buffer= Texture(size);
	compositing_glossiness_buffer= Texture(size);
	compositing_normal_buffer= Texture(size);
	compositing_depth_buffer= DepthTexture(size);

	compositing_framebuffer.Bind();
	compositing_framebuffer.AttachColorTexture(compositing_diffuse_color_buffer, 0);
	compositing_framebuffer.AttachColorTexture(compositing_glossiness_buffer, 1);
	compositing_framebuffer.AttachColorTexture(compositing_normal_buffer, 2);
	compositing_framebuffer.AttachDepthTexture(compositing_depth_buffer);
	compositing_framebuffer.CheckCompleteness();

	ShaderProgram *shader_program= ShaderProgram::Retrieve("gbuffer.program");
	glBindFragDataLocation(shader_program->GetHandle(), 0, "diffuse_color");
	glBindFragDataLocation(shader_program->GetHandle(), 1, "glossiness");
	glBindFragDataLocation(shader_program->GetHandle(), 2, "normal");


	phong_framebuffer.Bind();

	phong_color_buffer= Texture(size);
	phong_framebuffer.AttachColorTexture(phong_color_buffer, 0);

	phong_depth_buffer= DepthTexture(size);
	phong_framebuffer.AttachDepthTexture(phong_depth_buffer);

	phong_framebuffer.CheckCompleteness();


	shader_program= ShaderProgram::Retrieve("phong.program");
	shader_program->Use();
	shader_program->SetUniformInt("diffuse_buffer", 1);
	shader_program->SetUniformInt("glossiness_buffer", 2);
	shader_program->SetUniformInt("normal_buffer", 3);
	shader_program->SetUniformInt("depth_buffer", 4);
	shader_program->SetUniformInt("indirect_buffer", 5);
	shader_program->SetUniformInt("shadow_map", 6);
	shader_program->SetUniformMatrix4x4f("projection_transform", Transform::MakeProjectionTransform(FOV, Math::GetAspectRatio((float)size.x, (float)size.y)));

	
	indirect_light_texture= Texture(size, 5);
	indirect_light_texture_was_modified= false;


	initialized= true;
}

void RasterCamera::ResizeResizables(Vec2i size)
{
	diffuse_color_buffer.Size= size;
	glossiness_buffer.Size= size;
	normal_buffer.Size= size;
	depth_buffer.Size= size;
	
	compositing_diffuse_color_buffer.Size= size;
	compositing_glossiness_buffer.Size= size;
	compositing_normal_buffer.Size= size;
	compositing_depth_buffer.Size= size;

	phong_color_buffer.Size= size;
	phong_depth_buffer.Size= size;
	
	indirect_light_texture.Size= size;
}

void RasterCamera::Update()
{
	Camera::Update();

	ray_camera_is_invalid= true;
	compositing_camera_is_invalid= true;
}

void RasterCamera::GenerateIndirectLightTexture(Scene *scene, Vec2i size)
{
	if(ray_camera_is_invalid)
	{
		ray_camera.AssumeOrientation(*this);
		ray_camera_is_invalid= false;
	}

	indirect_light_photo= ray_camera.TakePhoto(*scene, size, Photo::Type::FullColor);
	indirect_light_texture_was_modified= true;
}

void RasterCamera::GenerateCompositingBuffers(Scene *scene, Vec2i size)
{
	if(compositing_camera_is_invalid)
		compositing_camera.AssumeOrientation(*this);

	compositing_camera.TakePhoto(*scene, size, Photo::Type::DiffuseColor);//Switch to TakePhotos()

	compositing_buffers_were_modified= true;

}

RasterCamera::RasterCamera(float fov, Vec3f position)
	: Camera(fov, position), ray_camera(fov, position), compositing_camera(fov, position), shadow_camera(Math::DegreesToRadians(70), Vec3f())
{
	animation_timer.Start();

	UseOrthographicProjection();
	ray_camera.UseOrthographicProjection();
	compositing_camera.UseOrthographicProjection();
}

RasterCamera::RasterCamera(float fov, Vec3f position, Vec3f look_at_position)
	: Camera(fov, position), ray_camera(fov, position), compositing_camera(fov, position), shadow_camera(Math::DegreesToRadians(70), Vec3f())
{
	animation_timer.Start();

	LookAt(look_at_position);

	UseOrthographicProjection();
	ray_camera.UseOrthographicProjection();
	compositing_camera.UseOrthographicProjection();
}

RasterCamera::~RasterCamera()
{
#ifndef NO_RAYTRACING
	raytracing_thread->join();
	delete raytracing_thread;

	if(compositing_thread!= nullptr)
	{
		compositing_thread->join();
		delete compositing_thread;
	}
#endif
}

PhotoBook RasterCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	Initialize(size);
	ResizeResizables(size);
	CatchUp();


	animation_timer.Pause();
	float elapsed_seconds= animation_timer.GetElapsedSeconds();
	animation_timer.Start();

#ifndef NO_RAYTRACING
	if(indirect_light_texture_was_modified || raytracing_thread== nullptr)
	{
		if(indirect_light_texture_was_modified)
		{
			raytracing_thread->join();
			delete raytracing_thread;

			indirect_light_texture.UploadImage(indirect_light_photo.GetImage());
			indirect_light_texture_was_modified= false;
		}

		raytracing_thread= new std::thread(&RasterCamera::GenerateIndirectLightTexture, this, &scene, size);
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

			compositing_diffuse_color_buffer.UploadImage(compositing_camera.GetDiffuseMap());
			compositing_glossiness_buffer.UploadImage(compositing_camera.GetGlossinessMap());
			compositing_normal_buffer.UploadImage(compositing_camera.GetNormalMap());
			compositing_depth_buffer.UploadImage(compositing_camera.GetDepthMap());

			compositing_buffers_were_modified= false;
		}

		if(compositing_camera_is_invalid)
		{
			compositing_thread= new std::thread(&RasterCamera::GenerateCompositingBuffers, this, &scene, size);

			compositing_camera_is_invalid= false;
		}
	}
#endif

#ifndef NO_RAYTRACING
	glBindFramebuffer(GL_READ_FRAMEBUFFER, compositing_framebuffer.GetHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer_framebuffer.GetHandle());

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	gbuffer_framebuffer.ActivateDefaultDrawBuffers();
	gbuffer_framebuffer.PrepareForDrawing();

#else
	gbuffer_framebuffer.PrepareForDrawing();
#endif

	ShaderProgram *gbuffer_shader_program= ShaderProgram::Retrieve("gbuffer.program");
	gbuffer_shader_program->Use();
	gbuffer_shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(Math::GetAspectRatio(size)).GetMatrix());

	scene.RasterizeConditionally(Prop::DrawFlags::RasterizeGbuffers);


	Light *light= scene.GetLights()[0];
	Vec3f light_position= light->GetPosition();
	Vec3f raster_light_position= light_position;
	shadow_camera.Position= raster_light_position;
	shadow_camera.LookAt(Vec3f(1, 1, 0));
	DepthTexture shadow_map= shadow_camera.TakePhoto(scene, Vec2i(2048, 2048), Photo::Type::Depth).GetTexture();
	shadow_map.BindToIndex(6);


	phong_framebuffer.PrepareForDrawing();

	ShaderProgram *phong_shader_program= ShaderProgram::Retrieve("phong.program");
	phong_shader_program->Use();
	phong_shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(Math::GetAspectRatio(size)).GetMatrix());
	phong_shader_program->SetUniformVector3f("camera_position", Position);
	phong_shader_program->SetUniformVector3f("light_position", raster_light_position);
	phong_shader_program->SetUniformVector3f("light_intensity", light->GetIntensity());
	phong_shader_program->SetUniformMatrix4x4f("shadow_camera_transform", shadow_camera.GetProjectedTransform(shadow_map.GetAspectRatio()).GetMatrix());
	phong_shader_program->SetUniformMatrix4x4f("transform", Transform());
	phong_shader_program->SetUniformMatrix4x4f("texture_transform", Transform());
	phong_shader_program->SetUniformInt("camera_is_orthographic", IsOrthographic());

	RasterizeFullScreenQuad();


	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::FullColor, Photo::Type::Depth };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::FullColor: photo_book[type]= phong_color_buffer; break;
			case Photo::Type::Depth: photo_book[type]= depth_buffer; break;

			case Photo::Type::DiffuseColor:
			case Photo::Type::SpecularColor: 
			case Photo::Type::Normal:
			default: break;
			}

	return photo_book;
}
