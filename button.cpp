#include "Button.h"


Button::Button(string image_filename, Roundness roundness, Pane *content)
	: ninepatch(image_filename, roundness)
{
	ninepatch.SetParent(this);

	SetContent(content);
}

void Button::SetContent(Pane *content_)
{
	content= content_;
	if(content== nullptr)
		return;

	content->SetParent(this);
	content->Offset= Vec2f(0.05f, 0.05f);
	content->Size= Vec2f(0.9f, 0.9f);
}

void Button::Draw()
{
	ninepatch.Draw();

	if(content!= nullptr)
		content->Draw();
}
