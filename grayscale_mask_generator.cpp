#include "GrayscaleMaskGenerator.h"
#include "Shader.h"
#include "Rasterizable.h"


GrayscaleMaskGenerator::GrayscaleMaskGenerator(Vec2i size)
{
	framebuffer.AttachDepthTexture(DepthTexture(size));
}

void GrayscaleMaskGenerator::SetSourceTexture(Texture texture)
{
	source= texture;
}

void GrayscaleMaskGenerator::SetDestinationTexture(Texture texture)
{
	destination= texture;

	framebuffer.AttachColorTexture(destination, 0);
}

Texture GrayscaleMaskGenerator::GetDestinationTexture()
{
	return destination;
}

Texture GrayscaleMaskGenerator::GenerateGrayscaleMask_Quad(Transform transform)
{
	if(source.GetHandle()== -1)
	{
		ColorImage white_image(framebuffer.GetSize(), Pixel(255, 255, 255, 255));
		SetSourceTexture(Texture(white_image));
	}

	if(destination.GetHandle()== -1)
		SetDestinationTexture(Texture(framebuffer.GetSize()));

	ShaderProgram *shader_program= ShaderProgram::Retrieve("quad.program");
	shader_program->Use();
	shader_program->SetUniformInt("draw_white", true);
	shader_program->SetUniformMatrix4x4f("transform", transform);
	shader_program->SetUniformMatrix4x4f("texture_transform", Transform());

	Framebuffer current_framebuffer= Framebuffer::GetCurrentFramebuffer();
	framebuffer.PrepareForDrawing();
	source.BindToIndex(8);

	glDisable(GL_DEPTH_TEST);
	FullScreenQuad::RasterizeInstance();
	glEnable(GL_DEPTH_TEST);

	shader_program->SetUniformInt("draw_white", false);
	current_framebuffer.Bind();

	return destination;
}

Texture GrayscaleMaskGenerator::GenerateGrayscaleMask_Mesh(Mesh * mesh, Transform transform)
{
	assert(false && "GrayscaleMaskGenerator::GenerateGrayscaleMask_Mesh(): Not implemented");

	return destination;
}
