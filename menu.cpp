#include "Menu.h"
#include "UI.h"


vector<Menu *> Menu::menus;
std::stack<Menu *> Menu::menu_stack;
TreeFrame Menu::default_frame= TreeFrame(Vec2f(0.1f, 0.1f), Vec2f(0.8f, 0.8f));
Interface * Menu::host= nullptr;

AboutDialog * Menu::about_dialog= new AboutDialog();
PauseMenu * Menu::pause_menu= new PauseMenu();

void Menu::Show()
{
	if(menu_stack.top()!= this)
		Open();
	else
		Pane::Show();
}

Menu::Menu()
{
	SetParent(&default_frame);
	Hide();

	menus.push_back(this);
}

void Menu::Open()
{
	if(menu_stack.size()> 0)
		menu_stack.top()->Hide();
	else if(host!= nullptr)
		host->Deactivate();
	menu_stack.push(this);
	Show();
}

void Menu::Back()
{
	menu_stack.top()->Hide();
	menu_stack.pop();
	if(menu_stack.size()> 0)
		menu_stack.top()->Show();
	else if(host!= nullptr)
		host->Activate();
}

void Menu::SetHost(Interface *host_)
{
	Menu::host= host_;
}

void Menu::Draw_Menus()
{
	for(Menu *menu : menus)
		menu->Draw();
}

void Menu::HandleEvent_Menus(Event *event_)
{
	for(Menu *menu : menus)
		menu->HandleEvent(event_);
}


OpenButton::OpenButton(string ninepatch_filename, string text, int text_size, Color text_color, Menu *menu_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_size, text_color, roundness)
{
	menu= menu_;
}

void OpenButton::MouseLeftClick()
{
	menu->Open();
}


BackButton::BackButton(string ninepatch_filename, string text, int text_size, Color text_color, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_size, text_color, roundness)
{
	
}

void BackButton::MouseLeftClick()
{
	Menu::Back();
}


