#ifndef PS_BUTTON
#define PS_BUTTON

#include "Pane.h"
#include "Ninepatch.h"


class Button : public Ninepatch
{
	Pane *content= nullptr;

public:
	//Ninepatch feature needs better interface, passing in a image filename is not very good.
	//Leaving as is for now because Styles will probably fix this issue.***
	Button(string ninepatch_filename, Roundness roundness= Roundness::Normal, Pane *content= nullptr);

	void SetContent(Pane *content);

	virtual void Draw();
};


class TextButton : public Button
{
	TextPane text_pane;

	using Button::SetContent;

public:
	TextPane * GetTextPane();

	TextButton(string ninepatch_filename, string text, Color font_color, Roundness roundness= Roundness::Normal);

	virtual void Draw();
};


class VisibilityToggleButton : public TextButton
{
	Drawable *drawable;

public:
	VisibilityToggleButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};


class ShowButton : public TextButton
{
	Drawable *drawable;

public:
	ShowButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};


class HideButton : public TextButton
{
	Drawable *drawable;

public:
	HideButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};

#endif