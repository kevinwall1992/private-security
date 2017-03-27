#include "Interface.h"


void Interface::ClearInteractionState()
{
	is_hovered= false;
	is_pressed= false;

	for(Gizmo *gizmo : gizmo_components)
	{
		Interface *interface_= dynamic_cast<Interface *>(gizmo);

		if(interface_!= nullptr)
			interface_->ClearInteractionState();
	}
}

void Interface::AddComponent(Interface *interface_)
{
	drawable_components.push_back(interface_);
	gizmo_components.push_back(interface_);
}

void Interface::AddComponent(Drawable *drawable)
{
	drawable_components.push_back(drawable);
}

void Interface::AddComponent(Gizmo *gizmo)
{
	gizmo_components.push_back(gizmo);
}

void Interface::RemoveComponent(Interface *interface_)
{
	Utility::RemoveElement<Drawable *>(drawable_components, interface_);
	Utility::RemoveElement<Gizmo *>(gizmo_components, interface_);
}

void Interface::RemoveComponent(Drawable *drawable)
{
	Utility::RemoveElement(drawable_components, drawable);
}

void Interface::RemoveComponent(Gizmo *gizmo)
{
	Utility::RemoveElement(gizmo_components, gizmo);
}

void Interface::ReplaceComponent(Interface *interface_, Interface *replacement_interface)
{
	Utility::ReplaceElement<Drawable *>(drawable_components, interface_, replacement_interface);
	Utility::ReplaceElement<Gizmo *>(gizmo_components, interface_, replacement_interface);
}

void Interface::ReplaceComponent(Drawable *drawable, Drawable *replacement_drawable)
{
	Utility::ReplaceElement(drawable_components, drawable, replacement_drawable);
}

void Interface::ReplaceComponent(Gizmo *gizmo, Gizmo *replacement_gizmo)
{
	Utility::ReplaceElement(gizmo_components, gizmo, replacement_gizmo);
}

void Interface::Draw_Components()
{
	if(IsHidden())
		return;

	for(Drawable *drawable : drawable_components)
		drawable->Draw();
}

void Interface::HandleEvent_Components(Event *event_)
{
	if(!IsActive())
		return;

	for(Gizmo *gizmo : gizmo_components)
		gizmo->HandleEvent(event_);
}

void Interface::HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event)
{
	if(IsInside(mouse_button_event->GetMousePosition()))
	{
		switch(mouse_button_event->GetType())
		{
		case EventType::Down:
			if(mouse_button_event->GetButton()== MouseButtonType::Left)
				is_pressed= true;
			break;

		case EventType::Up:
			switch(mouse_button_event->GetButton())
			{
			case MouseButtonType::Left: MouseLeftClick(); is_pressed= false; break;
			case MouseButtonType::Middle: MouseMiddleClick(); break;
			case MouseButtonType::Right: MouseRightClick(); break;
			}
			break;
		}
	}

	Gizmo::HandleMouseButtonEvent(mouse_button_event);
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
		is_hovered= true;
	}
	else if(IsInside(previous_mouse_position))
	{
		MouseOut();
		is_hovered= false;
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

void Interface::MouseLeftClick()
{
}

void Interface::MouseMiddleClick()
{
}

void Interface::MouseRightClick()
{
}

bool Interface::IsHovered()
{
	return is_hovered;
}

bool Interface::IsPressed()
{
	return is_pressed;
}

void Interface::Activate()
{
	Gizmo::Activate();
}

void Interface::Deactivate()
{
	Gizmo::Deactivate();

	ClearInteractionState();
}

/*void Interface::Activate()
{
	is_focused= true;
}

void Interface::Deactivate()
{
	is_focused= false;
	is_hovered= false;
	is_pressed= false;
}

bool Interface::IsActivateed()
{
	return is_focused;
}*/

void Interface::Show()
{
	Drawable::Show();

	activate_on_draw= true;
}

void Interface::Hide()
{
	Drawable::Hide();

	Deactivate();
}

void Interface::Draw()
{
	if(IsHidden())
		return;

	Draw_Components();

	if(activate_on_draw)
	{
		Activate();
		activate_on_draw= false;
	}
}

void Interface::HandleEvent(Event * event_)
{
	if(!DoesSkip(event_))
	{
		Gizmo::HandleEvent(event_);
		HandleEvent_Components(event_);
	}
}
