#include "Gizmo.h"


void Gizmo::HandleKeyboardEvent(ButtonEvent *button_event)
{
	ButtonType button= button_event->GetButton();
	EventType event_type= button_event->GetType();
	float t= button_event->GetElapsedSeconds();

	switch(event_type)
	{
		case EventType::Up:		KeyUp(button); break;
		case EventType::Down:	KeyDown(button); break;
		case EventType::Hold:	KeyHold(button, t); break;
	}
}

void Gizmo::KeyUp(ButtonType button) {}
void Gizmo::KeyDown(ButtonType button) {}
void Gizmo::KeyHold(ButtonType button, float t) {}

void Gizmo::HandleMouseEvent(MouseEvent *mouse_event)
{
	MouseScrollEvent *mouse_scroll_event= dynamic_cast<MouseScrollEvent *>(mouse_event);
	if(mouse_scroll_event!= nullptr)
	{
		MouseScroll(mouse_scroll_event->GetScrollAmount());
		return;
	}

	MouseMotionEvent *mouse_motion_event= dynamic_cast<MouseMotionEvent *>(mouse_event);
	if(mouse_motion_event!= nullptr)
	{
		HandleMouseMotionEvent(mouse_motion_event);
		return;
	}

	MouseButtonEvent *mouse_button_event= dynamic_cast<MouseButtonEvent *>(mouse_event);
	if(mouse_button_event!= nullptr)
	{
		HandleMouseButtonEvent(mouse_button_event);
		return;
	}

	
}

void Gizmo::HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event)
{
	MouseMotion(mouse_motion_event->GetMousePosition(), mouse_motion_event->GetMouseDisplacement());

	MouseDragEvent *mouse_drag_event= dynamic_cast<MouseDragEvent *>(mouse_motion_event);
	if(mouse_drag_event!= nullptr)
	{
		switch(mouse_drag_event->GetMouseButton())
		{
			case MouseButtonType::Left: MouseDragLeft(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
			case MouseButtonType::Middle: MouseDragMiddle(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
			case MouseButtonType::Right: MouseDragRight(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
		}

		return;
	}
}

void Gizmo::HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event)
{
	float t= mouse_button_event->GetElapsedSeconds();

	switch(mouse_button_event->GetMouseButton())
	{
	case MouseButtonType::Left:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseLeftUp(mouse_button_event->GetMousePosition()); break;
		case EventType::Down:			MouseLeftDown(mouse_button_event->GetMousePosition()); break;
		case EventType::DoubleClick:	MouseLeftDoubleClick(mouse_button_event->GetMousePosition()); break;
		case EventType::Hold:			MouseLeftHold(mouse_button_event->GetMousePosition(), t); break;
		default: break;
		}
		break;

	case MouseButtonType::Middle:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseMiddleUp(mouse_button_event->GetMousePosition()); break;
		case EventType::Down:			MouseMiddleDown(mouse_button_event->GetMousePosition()); break;
		case EventType::DoubleClick:	MouseMiddleDoubleClick(mouse_button_event->GetMousePosition()); break;
		case EventType::Hold:			MouseMiddleHold(mouse_button_event->GetMousePosition(), t); break;
		default: break;
		}
		break;

	case MouseButtonType::Right:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseRightUp(mouse_button_event->GetMousePosition()); break;
		case EventType::Down:			MouseRightDown(mouse_button_event->GetMousePosition()); break;
		case EventType::DoubleClick:	MouseRightDoubleClick(mouse_button_event->GetMousePosition()); break;
		case EventType::Hold:			MouseRightHold(mouse_button_event->GetMousePosition(), t); break;
		default: break;
		}
		break;

	default: break;
	}
}

void Gizmo::MouseLeftUp(Vec2f mouse_position) {}
void Gizmo::MouseLeftDown(Vec2f mouse_position) {}
void Gizmo::MouseLeftHold(Vec2f mouse_position, float t) {}
void Gizmo::MouseLeftDoubleClick(Vec2f mouse_position) {}
void Gizmo::MouseMiddleUp(Vec2f mouse_position) {}
void Gizmo::MouseMiddleDown(Vec2f mouse_position) {}
void Gizmo::MouseMiddleHold(Vec2f mouse_position, float t) {}
void Gizmo::MouseMiddleDoubleClick(Vec2f mouse_position) {}
void Gizmo::MouseRightUp(Vec2f mouse_position) {}
void Gizmo::MouseRightDown(Vec2f mouse_position) {}
void Gizmo::MouseRightHold(Vec2f mouse_position, float t) {}
void Gizmo::MouseRightDoubleClick(Vec2f mouse_position) {}
void Gizmo::MouseScroll(int scroll_amount) {}
void Gizmo::MouseMotion(Vec2f mouse_position, Vec2f mouse_displacement) {}
void Gizmo::MouseDragLeft(Vec2f mouse_position, Vec2f mouse_displacement) {}
void Gizmo::MouseDragMiddle(Vec2f mouse_position, Vec2f mouse_displacement) {}
void Gizmo::MouseDragRight(Vec2f mouse_position, Vec2f mouse_displacement) {}

void Gizmo::HandleEvent(Event *event_)
{
	//Some dynamic_cast helpers would be nice (also further up)***
	MouseEvent *mouse_event= dynamic_cast<MouseEvent *>(event_);
	if(mouse_event!= nullptr)
	{
		HandleMouseEvent(mouse_event);
		return;
	}

	ButtonEvent *button_event= dynamic_cast<ButtonEvent *>(event_);
	if(button_event!= nullptr)
	{
		HandleKeyboardEvent(button_event);
		return;
	}
}