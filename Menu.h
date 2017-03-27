#ifndef PS_MENU
#define PS_MENU

#include "Pane.h"
#include "Button.h"


class PauseMenu;
class AboutDialog;

class Menu : public Pane
{
	static vector<Menu *> menus;
	static std::stack<Menu *> menu_stack;
	static TreeFrame default_frame;
	static Interface *host;

protected:
	virtual void Show();

public:
	Menu();

	virtual void Open();
	static void Back();

	//Inconsistent with System, but I think this will be removed 
	//when we implement ui files***
	static PauseMenu *pause_menu;
	static AboutDialog *about_dialog;

	static void SetHost(Interface *host_);

	static void Draw_Menus();
	static void HandleEvent_Menus(Event *event_);
};


class OpenButton : public TextButton
{
	Menu *menu;

public:
	OpenButton(string ninepatch_filename, string text, int text_size, Color text_color, Menu *menu, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};


class BackButton : public TextButton
{
public:
	BackButton(string ninepatch_filename, string text, int text_size, Color text_color, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};

#endif
