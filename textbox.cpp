#include "Textbox.h"
#include "InputSystem.h"


Texture Textbox::Glyph::atlas;

void Textbox::Glyph::GenerateAtlas()
{
	if(atlas.GetHandle()!= -1)
		return;

	Font *font= Font::Monospace;

	ColorImage image= font->RenderLine(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 24, Color::Black);
	atlas= image;
	image.Free();
}

int Textbox::Glyph::GetAtlasIndex(char character)
{
	return character- ' ';
}

Transform Textbox::Glyph::GetTextureTransform()
{
	GenerateAtlas();

	float character_width= 1.0f/ (GetAtlasIndex('~')+ 1);
	int character_atlas_index= GetAtlasIndex(character);

	return Transform().Scale(Vec3f(character_width, -1, 1)).Translate(Vec3f(character_atlas_index* character_width, 0, 0));
}

Textbox::Glyph::Glyph(char character_)
{
	character= character_;
}

char Textbox::Glyph::GetCharacter()
{
	return character;
}

Texture Textbox::Glyph::GetTexture()
{
	return atlas;
}

void Textbox::Glyph::Draw()
{
	TexturePane::Draw();
}

void Textbox::AddGlyph(Glyph *glyph)
{
	AddComponent(glyph);
	glyphs.push_back(glyph);

	PlaceGlyphs();
}

void Textbox::RemoveGlyph(Glyph *glyph)
{
	for(unsigned int i= 0; i< glyphs.size(); i++)
	{
		if(glyph== glyphs[i])
		{
			glyphs.erase(glyphs.begin()+ i);
			break;
		}
	}

	RemoveComponent(glyph, true);

	PlaceGlyphs();
}

void Textbox::PlaceGlyphs()
{
	float cumulative_offset= 0.05f;

	for(Glyph *glyph : glyphs)
	{
		float horizontal_size= 0.0333f;
		glyph->Size= Vec2f(horizontal_size, 0.333f);
		glyph->Offset= Vec2f(cumulative_offset, 0.333f);
		cumulative_offset+= horizontal_size;
	}
}

bool Textbox::DoesConsume(Event *event_)
{
	return Ninepatch::DoesConsume(event_) || (IsHovered() && Utility::IsType<ButtonEvent>(event_));
}

void Textbox::KeyUp(ButtonType button)
{
	if(!IsHovered())
		return;

	if(button== ButtonType::Backspace)
		RemoveGlyph(glyphs[glyphs.size()- 1]);
	else
	{
		char character;

		switch(button)
		{
		case ButtonType::A: case ButtonType::B: case ButtonType::C: case ButtonType::D:
		case ButtonType::E: case ButtonType::F: case ButtonType::G: case ButtonType::H:
		case ButtonType::I: case ButtonType::J: case ButtonType::L: case ButtonType::M:
		case ButtonType::N: case ButtonType::O: case ButtonType::P: case ButtonType::Q:
		case ButtonType::R: case ButtonType::S: case ButtonType::T: case ButtonType::U:
		case ButtonType::V: case ButtonType::W: case ButtonType::X: case ButtonType::Y:
		case ButtonType::Z:
			character= 'a'+ (button- ButtonType::A); break;

		case ButtonType::CapitalA: case ButtonType::CapitalB: case ButtonType::CapitalC: case ButtonType::CapitalD:
		case ButtonType::CapitalE: case ButtonType::CapitalF: case ButtonType::CapitalG: case ButtonType::CapitalH:
		case ButtonType::CapitalI: case ButtonType::CapitalJ: case ButtonType::CapitalL: case ButtonType::CapitalM:
		case ButtonType::CapitalN: case ButtonType::CapitalO: case ButtonType::CapitalP: case ButtonType::CapitalQ:
		case ButtonType::CapitalR: case ButtonType::CapitalS: case ButtonType::CapitalT: case ButtonType::CapitalU:
		case ButtonType::CapitalV: case ButtonType::CapitalW: case ButtonType::CapitalX: case ButtonType::CapitalY:
		case ButtonType::CapitalZ:
			character= 'A'+ (button- ButtonType::CapitalA); break;

		case ButtonType::Zero: case ButtonType::One: case ButtonType::Two: case ButtonType::Three: case ButtonType::Four: 
		case ButtonType::Five: case ButtonType::Six: case ButtonType::Seven: case ButtonType::Eight: case ButtonType::Nine: 
			character= '0'+ (button- ButtonType::Zero); break;

		case ButtonType::ExclaimationMark: character= '!'; break;
		case ButtonType::At: character= '@'; break;
		case ButtonType::Pound: character= '#'; break;
		case ButtonType::Dollar: character= '$'; break;
		case ButtonType::Percent: character= '%'; break;
		case ButtonType::Caret: character= '^'; break;
		case ButtonType::And: character= '&'; break;
		case ButtonType::Asterisk: character= '*'; break;
		case ButtonType::LeftParenthesis: character= '('; break;
		case ButtonType::RightParenthesis: character= ')'; break;
		case ButtonType::Minus: character= '-'; break;
		case ButtonType::Underscore: character= '_'; break;
		case ButtonType::Equals: character= '='; break;
		case ButtonType::Plus: character= '+'; break;
		case ButtonType::Apostrophe: character= '\''; break;
		case ButtonType::QuotationMark: character= '\"'; break;
		case ButtonType::Comma: character= ','; break;
		case ButtonType::Period: character= '.'; break;
		case ButtonType::ForwardSlash: character= '/'; break;
		case ButtonType::QuestionMark: character= '?'; break;

		default: return;
		}

		AddGlyph(new Glyph(character));
	}
}

void Textbox::MouseIn()
{
	Ninepatch::MouseIn();

	Focus();
}

void Textbox::MouseOut()
{
	Ninepatch::MouseOut();

	Defocus();
}

Textbox::Textbox(string text)
	: Ninepatch("nine.png", Roundness::Normal)
{
	float cumulative_offset= 0.05f;

	SetText(text);
}

void Textbox::SetText(string text)
{
	while(glyphs.size()> 0)
		RemoveGlyph(glyphs[0]);

	for(char character : text)
		AddGlyph(new Glyph(character));
}

string Textbox::GetText()
{
	string text;

	for(Glyph *glyph : glyphs)
		text+= glyph->GetCharacter();

	return text;
}
