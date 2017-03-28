#include "Dialog.h"


Dialog::Dialog(string ninepatch_filename, string text, Color text_color, Ninepatch::Roundness roundness)
	: ninepatch(ninepatch_filename, roundness), 
	  text_pane(text, text_color, TextPane::Align::TopCenter),
	  close_button("nine.png", "Close", Color::Black)
{
	text_pane.Size= Vec2f(0.7f, 0.7f);
	text_pane.Offset= Vec2f(0.15f, 0.15f);

	close_button.Size= Vec2f(0.7f, 0.25f);
	close_button.Offset= Vec2f(0.15f, 0.1f);

	AddComponent(&ninepatch);
	AddComponent(&text_pane);
	AddComponent(&close_button);
}

TextPane * Dialog::GetTextPane()
{
	return &text_pane;
}

void Dialog::Draw()
{
	if(IsHidden())
		return;

	Menu::Draw();
}
