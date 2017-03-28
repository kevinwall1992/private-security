#include "Drawables.h"
#include "Shader.h"
#include "Rasterizable.h"


void TextDrawable::GenerateTextTexture()
{
	//Should make mechanism to go directly from surface to texture***
	ColorImage image= font->RenderLine(text, font_size, color);
	text_texture= image;
	image.Free();
}

TextDrawable::TextDrawable(string text_, int font_size_, Color color_)
{
	text= text_;
	font_size= font_size_;
	color= color_;

	font= Font::Default;
}

Vec2i TextDrawable::GetTextureSize()
{
	return font->GetTextSize(text, font_size);
}

float TextDrawable::GetAspectRatio()
{
	return text_texture.GetAspectRatio();
}

void TextDrawable::SetTransform(Transform transform_)
{
	transform= transform_;
}

void TextDrawable::Draw()
{
	if(text_texture.GetHandle()== -1)
		GenerateTextTexture();

	ShaderProgram *shader_program= ShaderProgram::Retrieve("quad.program");
	shader_program->Use();
	shader_program->SetUniformMatrix4x4f("transform", transform);
	shader_program->SetUniformMatrix4x4f("texture_transform", Transform().Scale(Vec3f(1, -1, 0)).Translate(Vec3f(0, 1, 0)));;

	text_texture.BindToIndex(0);

	glDisable(GL_DEPTH_TEST);
	RasterizeFullScreenQuad();
	glEnable(GL_DEPTH_TEST);
}
