#include "Menu.h"
#include "UI.h"
#include "GarbageCan.h"


vector<Menu *> Menu::menus;
std::stack<Menu *> Menu::menu_stack;
TreeFrame Menu::default_frame= TreeFrame(Vec2f(0.1f, 0.1f), Vec2f(0.8f, 0.8f));

AboutDialog * Menu::about_dialog= nullptr;
PauseMenu * Menu::pause_menu= nullptr;

void Menu::InitializeMenus()
{
	if(pause_menu!= nullptr)
		return;

	about_dialog= new AboutDialog();
	pause_menu= new PauseMenu();
}

bool Menu::IsAMenuOpen()
{
	return menu_stack.size()> 0;
}

int Menu::GetOccurrenceCount(Menu *menu)
{
	int occurrence_count= 0;

	std::stack<Menu *> menu_stack_copy= menu_stack;

	while(menu_stack_copy.size()> 0)
	{
		if(menu_stack_copy.top()== menu)
			occurrence_count++;

		menu_stack_copy.pop();
	}

	return occurrence_count;
}

void Menu::Show()
{
	if(menu_stack.top()!= this)
		Open();
	else
		Pane::Show();
}

Menu::Menu(bool single_use_)
{
	TreeFrame::SetParent(&default_frame);
	Hide();
	MakeOrphan();

	single_use= single_use_;

	menus.push_back(this);
}

void Menu::Open()
{
	if(menu_stack.size()> 0)
		menu_stack.top()->Hide();
	menu_stack.push(this);
	Show();
}

void Menu::Back()
{
	if(menu_stack.top()->single_use && GetOccurrenceCount(menu_stack.top())== 1)
	{
		GarbageCan<Menu>::ThrowAway(menu_stack.top());
		Utility::RemoveElement(menus, menu_stack.top());
	}
	else
		menu_stack.top()->Hide();
	menu_stack.pop();
	if(menu_stack.size()> 0)
		menu_stack.top()->Show();
}

void Menu::KeyUp(ButtonType button)
{
	if(button== ButtonType::Esc)
		Back();
}

void Menu::Draw_Menus()
{
	for(Menu *menu : menus)
		menu->Draw();
}

void Menu::HandleEvent_Menus(Event *event_)
{
	vector<Menu *> menus_copy= menus;

	for(Menu *menu : menus_copy)
		menu->HandleEvent(event_);
}


OpenButton::OpenButton(string ninepatch_filename, string text, Color text_color, Menu *menu_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	menu= menu_;
}

void OpenButton::MouseLeftClick()
{
	menu->Open();
}


BackButton::BackButton(string ninepatch_filename, string text, Color text_color, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	
}

void BackButton::MouseLeftClick()
{
	Menu::Back();
}


