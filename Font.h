#ifndef PS_FONT
#define PS_FONT

#include "FileResource.h"
#include "Image.h"
#include "Color.h"

#include <SDL_ttf.h>
#include <map>


class Font : public FileResource<Font>
{
	std::map<int, TTF_Font *> ttf_fonts;

	Font(string filepath);

public:
	TTF_Font *GetTTF_Font(int font_size);

	int GetMaximumFontSize(string text, Vec2i pixel_size);
	Vec2i GetTextSize(string text, int font_size);

	ColorImage RenderLine(string text, int font_size, Color color);

	static string MakeFilepath(string filename);
	static vector<Font *> Parse(string filename);

	static Font *Default;
};

#endif