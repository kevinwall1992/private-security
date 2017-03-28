#ifndef PS_DIALOG
#define PS_DIALOG

#include "Pane.h"
#include "Ninepatch.h"
#include "Menu.h"


class Dialog : public Menu
{
	Ninepatch ninepatch;
	TextPane text_pane;
	BackButton close_button;

public:
	Dialog(string ninepatch_filename, string text, Color text_color= Color::Black, Ninepatch::Roundness roundness= Ninepatch::Roundness::Sharp);

	TextPane * GetTextPane();

	virtual void Draw();
};

#endif
