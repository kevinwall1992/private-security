#include "IconCamera.h"
#include "Shader.h"


void IconCamera::Initialize(Vec2i size)
{
	if(initialized)
		return;

	framebuffer.AttachColorTexture(color_texture= Texture(size), 0);
	framebuffer.AttachDepthTexture(depth_texture= DepthTexture(size));
	framebuffer.CheckCompleteness();

	initialized= true;
}

void IconCamera::ResizeResizables(Vec2i size)
{
	color_texture.Size= size;
	depth_texture.Size= size;
}

IconCamera::IconCamera(float fov, Vec3f position)
	: Camera(fov, position)
{
}

IconCamera::IconCamera(float fov, Vec3f position, Vec3f look_at_position)
	: Camera(fov, position)
{
	LookAt(look_at_position);
}

PhotoBook IconCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	Initialize(size);
	ResizeResizables(size);

	framebuffer.PrepareForDrawing();
	
	Light *light= scene.GetLights()[0];
	ShaderProgram *shader_program= ShaderProgram::Retrieve("icon.program");
	shader_program->Use();
	shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(Math::GetAspectRatio(size)).GetMatrix());
	shader_program->SetUniformVector3f("light_position", light->GetPosition());
	shader_program->SetUniformVector3f("light_intensity", light->GetIntensity());

	scene.Rasterize();


	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::FullColor, Photo::Type::Depth };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::FullColor: photo_book[type]= color_texture; break;
			case Photo::Type::Depth: photo_book[type]= depth_texture; break;

			case Photo::Type::DiffuseColor:
			case Photo::Type::SpecularColor: 
			case Photo::Type::Normal:
			default: break;
			}

	return photo_book;
}