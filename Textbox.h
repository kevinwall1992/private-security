#ifndef PS_TEXT_INPUT
#define PS_TEXT_INPUT

#include "Ninepatch.h"
#include "Texture.h"
#include "Font.h"

#include <SDL_ttf.h>

#include <string>
#include <vector>

using std::string;
using std::vector;


class Textbox : public Ninepatch
{
	class Glyph : public TexturePane
	{
		static Texture atlas;

		static void GenerateAtlas();
		static int GetAtlasIndex(char character);

		char character;

	protected:
		virtual Transform GetTextureTransform();

	public:
		Glyph(char character);

		char GetCharacter();

		virtual Texture GetTexture();

		virtual void Draw();
	};

	vector<Glyph *> glyphs;

	void AddGlyph(Glyph *glyph);
	void RemoveGlyph(Glyph *glyph);
	void PlaceGlyphs();

protected:
	virtual bool DoesConsume(Event *event_);

	virtual void KeyUp(ButtonType button);

	virtual void MouseIn();
	virtual void MouseOut();

public:
	Textbox(string text= "");

	void SetText(string text);
	string GetText();
};

#endif