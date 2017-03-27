#include "Dialog.h"


Dialog::Dialog(string ninepatch_filename, string text, int text_size, Color text_color, Ninepatch::Roundness roundness)
	: ninepatch(ninepatch_filename, roundness), 
	  text_pane(text, text_size, text_color),
	  close_button("nine.png", "Close", 24, Color::Black)
{
	text_pane.Size= Vec2f(1.0f, 0.1f);
	text_pane.Offset= Vec2f(0.0f, 0.8f);

	close_button.Size= Vec2f(0.8f, 0.25f);
	close_button.Offset= Vec2f(0.1f, 0.1f);

	AddComponent(&ninepatch);
	AddComponent(&text_pane);
	AddComponent(&close_button);
}