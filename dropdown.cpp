#include "Dropdown.h"

Dropdown::DropdownElement::DropdownElement(string text, Dropdown *dropdown_)
	: TextButton("nine.png", text, Color::Black)
{
	dropdown= dropdown_;

	DontIntersectParentBounds();
}

void Dropdown::DropdownElement::MouseLeftClick()
{
	dropdown->SetSelectedElement(this);
}

void Dropdown::SetSelectedElement(DropdownElement *element)
{
	selected_element= element;

	ClearPanes();
	AddPane(selected_element);

	if(IsFocused())
	{
		Defocus();

		SpawnInterfaceEvent(new DropdownInterfaceEvent(this, selected_element->GetText()));
	}
}

Dropdown::Dropdown()
	: ListPane(Direction::TopToBottom)
{

}

Dropdown::Dropdown(vector<string> texts)
	: ListPane(Direction::TopToBottom)
{
	AddElements(texts);
}

void Dropdown::AddElement(string text)
{
	DropdownElement *element= new DropdownElement(text, this);
	element->Size= Vec2f(1.0f, 1.0f);

	elements.push_back(element);

	if(IsFocused())
		AddPane(element);
	else if(elements.size()== 1)
	{
		AddPane(element);
		selected_element= element;
	}
}

void Dropdown::AddElements(vector<string> texts)
{
	for(string text : texts)
		AddElement(text);
}

void Dropdown::ClearElements()
{
	elements.clear();
	ClearPanes(true);
}

void Dropdown::SetSelectedElement(string text)
{
	for(DropdownElement *element : elements)
	{
		if(element->GetText()== text)
		{
			SetSelectedElement(element);
			break;
		}
	}
}

string Dropdown::GetSelectedElement()
{
	if(selected_element== nullptr)
		return "";

	return selected_element->GetText();
}

void Dropdown::Focus()
{
	if(IsFocused())
		return;

	ListPane::Focus();

	ClearPanes();
	for(DropdownElement *element : elements)
		AddPane(element);
}

void Dropdown::MouseLeftClick()
{
	if(!IsFocused())
	{
		Focus();
		DontRebroadcastCurrentEvent();
	}
}

Dropdown::DropdownInterfaceEvent::DropdownInterfaceEvent(Dropdown *dropdown_, string selected_element_)
	: InterfaceEvent(dropdown_)
{
	dropdown= dropdown_;
	selected_element= selected_element_;
}

Dropdown * Dropdown::DropdownInterfaceEvent::GetDropdown()
{
	return dropdown;
}

string Dropdown::DropdownInterfaceEvent::GetSelectedElement()
{
	return selected_element;
}
