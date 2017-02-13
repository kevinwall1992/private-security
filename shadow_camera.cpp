#include "ShadowCamera.h"
#include "Texture.h"
#include "Shader.h"

void ShadowCamera::Initialize(int width, int height)
{
	if(initialized)
		return;

	framebuffer.Bind();

	color_texture= Texture(width, height);
	depth_texture= DepthTexture(width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	framebuffer.AttachColorTexture(color_texture, 0);
	framebuffer.AttachDepthTexture(depth_texture);

	initialized= true;
}

void ShadowCamera::ResizeResizables(int width, int height)
{
	color_texture.Resize(width, height);
	depth_texture.Resize(width, height);
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

Photo ShadowCamera::TakePhoto(Scene &scene, int width, int height)
{
	Initialize(width, height);
	ResizeResizables(width, height);
	CatchUp();

	framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	ShaderProgram *shader_program= ShaderProgram::Retrieve("shadow.program");
	shader_program->Use();
	shader_program->SetUniformMatrix4x4f("camera_transform", GetProjectedTransform(width, height));
	shader_program->SetUniformVector3f("light_position", Position);

	//scene.RasterizeConditionally(Prop::DrawFlags::RasterizeGbuffers);
	scene.Rasterize();

	return Photo(depth_texture);
}
