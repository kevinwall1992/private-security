#include "Button.h"
#include "Drawables.h"


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
	content->Offset= Vec2f(0.05f, 0.05f);
	content->Size= Vec2f(0.9f, 0.9f);
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

TextButton::TextButton(string ninepatch_filename, string text, int font_size, Color font_color, Roundness roundness)
	: Button(ninepatch_filename, roundness, new TextPane(text, font_size, font_color))
{

}


VisibilityToggleButton::VisibilityToggleButton(string ninepatch_filename, string text, int text_size, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_size, text_color, roundness)
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

ShowButton::ShowButton(string ninepatch_filename, string text, int text_size, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_size, text_color, roundness)
{
	drawable= drawable_;
}

void ShowButton::MouseLeftClick()
{
	drawable->Show();
}

HideButton::HideButton(string ninepatch_filename, string text, int text_size, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_size, text_color, roundness)
{
	drawable= drawable_;
}

void HideButton::MouseLeftClick()
{
	drawable->Hide();
}
