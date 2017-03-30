#include "Button.h"
#include "Drawables.h"


Pane * Button::GetContent()
{
	return content;
}

Button::Button(string ninepatch_filename, Roundness roundness, Pane *content)
	: Ninepatch(ninepatch_filename, roundness)
{
	SetContent(content);
}

void Button::SetContent(Pane *content_)
{
	if(content_== nullptr)
		return;

	if(content!= nullptr)
		RemoveComponent(content);

	content= content_;
	content->Offset= Vec2f(0.25f, 0.25f);
	content->Size= Vec2f(0.5f, 0.5f);
	AddComponent(content);
}

void Button::Draw()
{
	Vec2f size= Size;
	Vec2f offset= Offset;
	if(IsHovered())
	{
		Size= size* 1.1f;
		Offset-= (Size- size)/ 2;
	}

	Ninepatch::Draw();

	Size= size;
	Offset= offset;
}

TextPane * TextButton::GetTextPane()
{
	return &text_pane;
}

TextButton::TextButton(string ninepatch_filename, string text, Color font_color, Roundness roundness)
	: text_pane(text, font_color), Button(ninepatch_filename, roundness)
{
	SetContent(&text_pane);
}

void TextButton::Draw()
{
	text_pane.LockFontSize();
	Button::Draw();
	text_pane.UnlockFontSize();
}


VisibilityToggleButton::VisibilityToggleButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void VisibilityToggleButton::MouseLeftClick()
{
	if(drawable->IsHidden())
		drawable->Show();
	else
		drawable->Hide();
}

ShowButton::ShowButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void ShowButton::MouseLeftClick()
{
	drawable->Show();
}

HideButton::HideButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void HideButton::MouseLeftClick()
{
	drawable->Hide();
}
