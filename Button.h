#ifndef PS_BUTTON
#define PS_BUTTON

#include "Pane.h"
#include "Ninepatch.h"
#include "Cycle.h"


class Button : public Ninepatch
{
	Pane *content= nullptr;
	SmoothCycle cycle;

protected:
	Pane * GetContent();

public:
	//Ninepatch feature needs better interface, passing in a image filename is not very good.
	//Leaving as is for now because Styles will probably fix this issue.***
	Button(string ninepatch_filename, Roundness roundness= Roundness::Normal, Pane *content= nullptr);

	virtual void MouseIn();
	virtual void MouseOut();

	void SetContent(Pane *content);

	virtual void MouseLeftClick();

	virtual void Draw();


	class ButtonInterfaceEvent : public InterfaceEvent
	{
		Button *button;

	public:
		ButtonInterfaceEvent(Button *button);

		Button * GetButton();
	};
};
typedef Button::ButtonInterfaceEvent ButtonInterfaceEvent;


class TextButton : public Button
{
	TextPane text_pane;

	using Button::SetContent;

public:
	TextPane * GetTextPane();

	string GetText();
	void SetText(string text);

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

class ToggleButton : public Button
{
	Color on_color, off_color;

	bool is_on;

public:
	ToggleButton(string ninepatch_filename, Roundness roundness, Color on_color, Color off_color, bool is_on= false);
	ToggleButton();

	void TurnOn();
	void TurnOff();

	bool IsOn();

	virtual void MouseLeftClick();
};

class SelectFileButton : public TextButton
{
	string filepath;
	vector<string> file_extensions;

public:
	SelectFileButton(string initial_filepath, vector<string> file_extensions);

	string GetFilepath();
	string GetFilename();

	void SetFilepath(string filepath);

	virtual void MouseLeftClick();


	class SelectFileButtonInterfaceEvent : public ButtonInterfaceEvent
	{
		string filepath;
		
	public:
		SelectFileButtonInterfaceEvent(SelectFileButton *button, string filepath);

		string GetFilepath();
		string GetFilename();
	};
};
typedef SelectFileButton::SelectFileButtonInterfaceEvent SelectFileButtonInterfaceEvent;



class ImageButton : public Button
{
	ImagePane image_pane;

public:
	ImageButton(string image_filename);
};

#endif