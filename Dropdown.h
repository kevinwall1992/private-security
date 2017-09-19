#ifndef PS_DROPDOWN
#define PS_DROPDOWN

#include "ListPane.h"

class Dropdown : public ListPane
{
	class DropdownElement : public TextButton
	{
		Dropdown *dropdown;

	public:
		DropdownElement(string text, Dropdown *dropdown);

		virtual void MouseLeftClick();
	};

	vector<DropdownElement *> elements;
	DropdownElement *selected_element;

	void SetSelectedElement(DropdownElement *element);

public:
	Dropdown();
	Dropdown(vector<string> texts);

	void AddElement(string text);
	void AddElements(vector<string> texts);
	void ClearElements();

	void SetSelectedElement(string text);
	string GetSelectedElement();

	virtual void Focus();

	virtual void MouseLeftClick();

	class DropdownInterfaceEvent : public InterfaceEvent
	{
		Dropdown *dropdown;
		string selected_element;

	public:
		DropdownInterfaceEvent(Dropdown *dropdown, string selected_element);

		Dropdown * GetDropdown();
		string GetSelectedElement();
	};
};
typedef Dropdown::DropdownInterfaceEvent DropdownInterfaceEvent;

#endif