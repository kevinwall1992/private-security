#include "Drawables.h"
#include "Shader.h"
#include "Rasterizable.h"


//TTF_Fonts should be wrapped in a FileResource subclass
void TextDrawable::GenerateTextTexture()
{
	if(!TTF_WasInit())
		TTF_Init();
 
	TTF_Font *font= TTF_OpenFont("data/assets/graphical/fonts/Roboto-Regular.ttf", font_size);

	SDL_Surface *surface= TTF_RenderText_Blended(font, text.c_str(), color.ToSDLColor());
	text_texture= Texture(Utility::MakeColorImageFromSDL_Surface(surface));
	SDL_FreeSurface(surface);
	

	TTF_CloseFont(font);
}

TextDrawable::TextDrawable(string text_, int font_size_, Color color_)
{
	text= text_;
	font_size= font_size_;
	color= color_;
}

Vec2i TextDrawable::GetTextureSize()
{
	return text_texture.Size;
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
	shader_program->SetUniformMatrix4x4f("texture_transform", Transform().Scale(Vec3f(1, -1, 0)));;

	text_texture.BindToIndex(0);

	glDisable(GL_DEPTH_TEST);
	RasterizeFullScreenQuad();
	glEnable(GL_DEPTH_TEST);
}
