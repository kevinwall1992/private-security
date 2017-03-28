#include "Font.h"

Font *Font::Default= new Font(Font::MakeFilepath("Roboto-Regular.ttf"));

Font::Font(string filepath)
	: FileResource(filepath)
{
	
}

TTF_Font *Font::GetTTF_Font(int font_size)
{
	if(!TTF_WasInit())
		TTF_Init();

	if(ttf_fonts[font_size]== nullptr)
		ttf_fonts[font_size]= TTF_OpenFont(GetFilepath().c_str(), font_size);

	return ttf_fonts[font_size];
}

int Font::GetMaximumFontSize(string text, Vec2i pixel_size)
{
	int font_size= 6;
	while(true)
	{
		Vec2i text_size= GetTextSize(text, font_size);

		if(text_size.x> pixel_size.x || text_size.y> pixel_size.y)
		{
			font_size--;
			break;
		}

		font_size++;
	}

	return font_size;
}

Vec2i Font::GetTextSize(string text, int font_size)
{
	Vec2i text_size;

	vector<string> lines= Utility::SplitString(text);
	for(unsigned int i= 0; i< lines.size(); i++)
	{
		Vec2i line_size;
		TTF_SizeText(GetTTF_Font(font_size), lines[i].c_str(), line_size.c+ 0, line_size.c+ 1);

		text_size.x= std::max(text_size.x, line_size.x);
		text_size.y+= line_size.y;
	}
	
	return text_size;
}

ColorImage Font::RenderLine(string line, int font_size, Color color)
{
	SDL_Surface *surface= TTF_RenderText_Blended(GetTTF_Font(font_size), line.c_str(), color.ToSDLColor());
	ColorImage image= Utility::MakeColorImageFromSDL_Surface(surface);
	SDL_FreeSurface(surface);

	return image;
}

string Font::MakeFilepath(string filename)
{
	return "data/assets/graphical/fonts/"+ filename;
}

vector<Font*> Font::Parse(string filename)
{
	return Utility::MakeVector(new Font(MakeFilepath(filename)));
}
