#include "RasterCamera.h"
#include "Shader.h"
#include "RayCamera.h"
#include "CompositingCamera.h"
#include "ShadowCamera.h"
#include "Viewport.h"
#include "GameSystem.h"


//#define NO_RAYTRACING

void RasterCamera::Initialize(Vec2i size)
{
	if(initialized)
		return;

	diffuse_color_buffer= Texture(size, 1);
	glossiness_buffer= Texture(size, 2);
	normal_buffer= Texture(size, 3);
	indirect_light_stencil= Texture(size, 7);
	depth_buffer= DepthTexture(size, 4);
	
	gbuffer_framebuffer.Bind();
	gbuffer_framebuffer.AttachColorTexture(diffuse_color_buffer, 0);
	gbuffer_framebuffer.AttachColorTexture(glossiness_buffer, 1);
	gbuffer_framebuffer.AttachColorTexture(normal_buffer, 2);
	gbuffer_framebuffer.AttachColorTexture(indirect_light_stencil, 3);
	gbuffer_framebuffer.AttachDepthTexture(depth_buffer);
	gbuffer_framebuffer.CheckCompleteness();

	ShaderProgram *shader_program= ShaderProgram::Retrieve("gbuffer.program");
	glBindFragDataLocation(shader_program->GetHandle(), 0, "diffuse_color");
	glBindFragDataLocation(shader_program->GetHandle(), 1, "glossiness");
	glBindFragDataLocation(shader_program->GetHandle(), 2, "normal");
	glBindFragDataLocation(shader_program->GetHandle(), 3, "indirect_light_stencil");


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
	shader_program->SetUniformInt("indirect_light_stencil_buffer", 7);
	shader_program->SetUniformMatrix4x4f("projection_transform", Transform::MakeProjectionTransform(FOV, Math::GetAspectRatio((float)size.x, (float)size.y)));

	downscale_framebuffer.AttachColorTexture(downscale_texture= Texture(GetDownscaleSize(size)), 0);
	downscale_framebuffer.AttachDepthTexture(downscale_depth_texture= DepthTexture(GetDownscaleSize(size)));

	//downscale_texture.BindToIndex(0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	
	indirect_light_texture= Texture(GetSceneViewPixelSize(GetDownscaleSize(size)), 5);
	indirect_light_texture_was_modified= false;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	initialized= true;
}

void RasterCamera::ResizeResizables(Vec2i size)
{
	diffuse_color_buffer.Size= size;
	glossiness_buffer.Size= size;
	normal_buffer.Size= size;
	indirect_light_stencil.Size= size;
	depth_buffer.Size= size;

	phong_color_buffer.Size= size;
	phong_depth_buffer.Size= size;

	downscale_texture.Size= GetDownscaleSize(size);
	downscale_depth_texture.Size= GetDownscaleSize(size);
}

void RasterCamera::GenerateIndirectLightTexture(Scene *scene, Vec2i size)
{
	Vec2i scene_photo_size= GetSceneViewPixelSize(size);

	if(ray_camera_is_invalid)
	{
		ray_camera.AssumeOrientation(*this);

		if(IsOrthographic())
		{
			ray_camera.LookAt(SnapToPixel((Vec3f)System::game.space.GetSize()/ Vec3f(2, 2, 2), size));
			ray_camera.FOV= FOV* (size.x> size.y ? (scene_photo_size.x/ (float)size.x) : (scene_photo_size.y/ (float)size.y));
		}
		ray_camera_is_invalid= false;
	}

	indirect_light_photo= ray_camera.TakePhoto(*scene, IsOrthographic() ? GetDownscaleSize(scene_photo_size) : size, Photo::Type::FullColor);
	indirect_light_texture_was_modified= true;
}

Vec2f RasterCamera::GetSceneViewWorldSize()
{
	Vec3f u= GetRight();
	u= Vec3f(abs(u.x), abs(u.y), abs(u.z));
	Vec3f v= GetUp();
	v= Vec3f(abs(v.x), abs(v.y), abs(v.z));

	return Vec2f(u.Dot(System::game.space.GetSize()), v.Dot(System::game.space.GetSize()));
}

Vec2i RasterCamera::GetSceneViewPixelSize(Vec2i photo_size)
{
	Vec2f pixel_size= GetSceneViewWorldSize()* GetWorldToPixelRatio(photo_size);
	int side= ((int)(std::max(pixel_size.x, pixel_size.y)/ 32+ 1))* 32;

	return Vec2i(side, side);
}

float RasterCamera::GetPixelToWorldRatio(Vec2i photo_size)
{
	return FOV/ photo_size.y;
}

float RasterCamera::GetWorldToPixelRatio(Vec2i photo_size)
{
	return 1/ GetPixelToWorldRatio(photo_size);
}

Vec2i RasterCamera::GetDownscaleSize(Vec2i size)
{
	return (Vec2f)size* downscale_factor;
}

RasterCamera::RasterCamera(float fov, Vec3f position)
	: Camera(fov, position), ray_camera(fov, position), shadow_camera(Math::DegreesToRadians(90), Vec3f()), light_cycle(6)
{
	
}

RasterCamera::RasterCamera(float fov, Vec3f focus, float pitch, float yaw)
	: Camera(fov, focus, pitch, yaw), ray_camera(fov, focus, pitch, yaw), shadow_camera(Math::DegreesToRadians(90), Vec3f()), light_cycle(6)
{
}

RasterCamera::~RasterCamera()
{
#ifndef NO_RAYTRACING
	raytracing_thread->join();
	delete raytracing_thread;
#endif
}

PhotoBook RasterCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	size= (Vec2f)size/ downscale_factor;

	Initialize(size);
	ResizeResizables(size);
	if(!ValidateFOV() || !ValidateRotation() || (!IsOrthographic() && !ValidatePosition()))
		ray_camera_is_invalid= true;

#ifndef NO_RAYTRACING
	if(indirect_light_texture_was_modified || raytracing_thread== nullptr)
	{
		if(indirect_light_texture_was_modified)
		{
			raytracing_thread->join();
			delete raytracing_thread;

			indirect_light_texture.Size= indirect_light_photo.GetImage().Size;
			indirect_light_texture.UploadImage(indirect_light_photo.GetImage());
			indirect_light_texture_was_modified= false;
		}

		raytracing_thread= new std::thread(&RasterCamera::GenerateIndirectLightTexture, this, &scene, size);
	}
#endif

	gbuffer_framebuffer.PrepareForDrawing();

	Vec3f position= Position;
	if(IsOrthographic())
		Position= SnapToPixel(Position, size);

	ShaderProgram *gbuffer_shader_program= ShaderProgram::Retrieve("gbuffer.program");
	gbuffer_shader_program->Use();
	gbuffer_shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(Math::GetAspectRatio(size)).GetMatrix());

	scene.RasterizeConditionally_All(Prop::DrawFlags::Direct);


	Light *light= scene.GetLights()[0];
	Vec3f light_position= light->GetPosition();
	Vec3f raster_light_position= light_position+ Transform().RotateAboutY((float)(M_PI* 2* light_cycle.GetMoment())).Apply(Vec3f(1.35, 0, 0));
	shadow_camera.Position= raster_light_position;
	shadow_camera.LookAt(Vec3f(15.5f, 0, 15.5f));
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

	if(IsOrthographic())
	{
		phong_shader_program->SetUniformFloat("indirect_buffer_scale", FOV/ ray_camera.FOV);

		Vec3f u= GetRight();
		Vec3f v= GetUp();
		Vec3f corner= (u* -ray_camera.FOV/ 2)+ (v* -ray_camera.FOV/ 2)+ (GetForward()* -ray_camera.FOV/ 2)+ ray_camera.Position;
		Vec3f world_corner_offset0= Position- corner;
		Vec3f world_corner_offset1= ray_camera.Position- corner;
		Vec2f image_corner_offset0= Vec2f(world_corner_offset0.Dot(u), world_corner_offset0.Dot(v))/ ray_camera.FOV;
		Vec2f image_corner_offset1= Vec2f(world_corner_offset1.Dot(u), world_corner_offset1.Dot(v))/ ray_camera.FOV;
		phong_shader_program->SetUniformVector2f("indirect_buffer_offset", (image_corner_offset0* (ray_camera.FOV/ FOV)- image_corner_offset1)/ (ray_camera.FOV/ FOV));

		Position= position;
	}
	else
	{
		phong_shader_program->SetUniformFloat("indirect_buffer_scale", 1);
		phong_shader_program->SetUniformVector2f("indirect_buffer_offset", Vec2f());
	}

	RasterizeFullScreenQuad();


	downscale_framebuffer.PrepareForDrawing();
	ShaderProgram *downscale_shader_program= ShaderProgram::Retrieve("quad.program");
	downscale_shader_program->Use();
	downscale_shader_program->SetUniformMatrix4x4f("transform", Transform());
	downscale_shader_program->SetUniformMatrix4x4f("texture_transform", Transform());
	phong_color_buffer.BindToIndex(0);
	RasterizeFullScreenQuad();


	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::FullColor, Photo::Type::Depth };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::FullColor: photo_book[type]= downscale_texture; break;
			case Photo::Type::Depth: photo_book[type]= depth_buffer; break;

			case Photo::Type::DiffuseColor:
			case Photo::Type::SpecularColor: 
			case Photo::Type::Normal:
			default: break;
			}

	return photo_book;
}
