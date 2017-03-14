#include "Interface.h"


void Interface::HandleEvent(Event * event_)
{
	if(!DoesSkip(event_))
	{
		Gizmo::HandleEvent(event_);
	}
		
}

void Interface::HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event)
{
	Vec2f mouse_position= mouse_motion_event->GetMousePosition();
	Vec2f previous_mouse_position= mouse_position- mouse_motion_event->GetMouseDisplacement();

	if(IsInside(mouse_position))
	{
		if(!IsInside(previous_mouse_position))
			MouseIn();

		MouseHover(mouse_position);
	}
	else if(IsInside(previous_mouse_position))
	{
		MouseOut();
	}

	Gizmo::HandleMouseMotionEvent(mouse_motion_event);
}

bool Interface::DoesSkip(Event * event_)
{
	return false;
}

bool Interface::DoesConsume(Event * event_)
{
	return false;
}

void Interface::MouseHover(Vec2f mouse_position) { }
void Interface::MouseIn() { }
void Interface::MouseOut() { }

void Interface::Focus()
{
	is_focused= true;
}

void Interface::Defocus()
{
	is_focused= false;
}

bool Interface::IsFocused()
{
	return is_focused;
}

void InterfaceContainer::HandleEvent(Event *event_)
{
	Interface::HandleEvent(event_);

	for(Interface *interface_ : interfaces)
	{
		interface_->HandleEvent(event_);
		if(DoesConsume(event_))
			break;
	}
}

bool InterfaceContainer::IsInside(Vec2f point)
{
	for(Interface *interface_ : interfaces)
		if(interface_->IsInside(point))
			return true;

	return false;
}

void InterfaceContainer::SendToFront(Interface * interface_)
{
	int index= -1;
	for(unsigned int i= 0; i< interfaces.size(); i++)
	{
		if(interfaces[i]== interface_)
		{
			index= i;
			break;
		}
	}

	if(index== -1)
	{
		assert(false && "InteraceContainer::SendToFront: Interface not found");
		return;
	}

	interfaces.erase(interfaces.begin()+ index);
	interfaces.insert(interfaces.begin(), interface_);
}

Interface * InterfaceContainer::GetInterface(Vec2f point)
{
	for(Interface *interface_ : interfaces)
		if(interface_->IsInside(point))
			return interface_;

	return nullptr;
}

void InterfaceContainer::AddInterface(Interface * interface_)
{
	interfaces.push_back(interface_);
}

void InterfaceContainer::Draw()
{
	for(Interface *interface_ : interfaces)
		interface_->Draw();
}

void WindowContainer::MouseLeftUp(Vec2f mouse_position)
{
	Interface *interface_= GetInterface(mouse_position);
	interface_->Focus();
	SendToFront(interface_);
}
