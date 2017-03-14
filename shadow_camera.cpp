#include "ShadowCamera.h"
#include "Texture.h"
#include "Shader.h"

void ShadowCamera::Initialize(Vec2i size)
{
	if(initialized)
		return;

	framebuffer.Bind();

	color_texture= Texture(size);
	depth_texture= DepthTexture(size);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	framebuffer.AttachColorTexture(color_texture, 0);
	framebuffer.AttachDepthTexture(depth_texture);

	initialized= true;
}

void ShadowCamera::ResizeResizables(Vec2i size)
{
	color_texture.Resize(size);
	depth_texture.Resize(size);
}

ShadowCamera::ShadowCamera(float fov_in_degrees, Vec3f position, Vec3f look_at_position)
	: Camera(fov_in_degrees, position)
{
	LookAt(look_at_position);
}

ShadowCamera::ShadowCamera(float fov_in_degrees, Vec3f position)
	: Camera(fov_in_degrees, position)
{

}

PhotoBook ShadowCamera::TakePhotos(Scene &scene, Vec2i size, Photo::Type types)
{
	Initialize(size);
	ResizeResizables(size);
	CatchUp();

	framebuffer.PrepareForDrawing();

	ShaderProgram *shader_program= ShaderProgram::Retrieve("shadow.program");
	shader_program->Use();
	shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(Math::GetAspectRatio(size)));
	shader_program->SetUniformVector3f("light_position", Position);

	scene.Rasterize();


	PhotoBook photo_book;

	vector<Photo::Type> all_types= { Photo::Type::Depth };
	for(Photo::Type type : all_types)
		if((type & types)== type)
			switch(type)
			{
			case Photo::Type::Depth: photo_book[type]= depth_texture; break;

			case Photo::Type::FullColor:
			case Photo::Type::DiffuseColor:
			case Photo::Type::SpecularColor: 
			case Photo::Type::Normal:
			default: break;
			}

	return photo_book;
}
