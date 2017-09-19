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

	bool single_use;

	//Consider changing AboutDialog and PauseMenu's constructors now that they aren't 
	//intialized at program start anymore. ####
	//static void InitializeMenus();

protected:
	static int GetOccurrenceCount(Menu *menu);

	virtual void Show();

public:
	Menu(bool single_use= false);

	virtual void Open();
	static void Back();

	virtual void KeyUp(ButtonType button);

	//Inconsistent with System, but I think this will be removed 
	//when we implement ui files
	static PauseMenu *pause_menu;
	static AboutDialog *about_dialog;

	static void Draw_Menus();
	static void HandleEvent_Menus(Event *event_);

	static void InitializeMenus();

	static bool IsAMenuOpen();
};


class OpenButton : public TextButton
{
	Menu *menu;

public:
	OpenButton(string ninepatch_filename, string text, Color text_color, Menu *menu, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};


class BackButton : public TextButton
{
public:
	BackButton(string ninepatch_filename, string text, Color text_color, Roundness roundness= Roundness::Normal);

	virtual void MouseLeftClick();
};

#endif
