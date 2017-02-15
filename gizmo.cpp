#include "Gizmo.h"
#include "Camera.h"


void Gizmo::HandleKeyboardEvent(ButtonEvent *button_event)
{
	Button::Enum button= button_event->GetButton();
	EventType::Enum event_type= button_event->GetType();
	float t= button_event->GetElapsedSeconds();

	switch(event_type)
	{
		case EventType::Up:		KeyUp(button, event_type); break;
		case EventType::Down:	KeyDown(button, event_type); break;
		case EventType::Hold:	KeyHold(button, event_type, t); break;
	}
}

void Gizmo::KeyUp(Button::Enum button, EventType::Enum event_type)
{

}

void Gizmo::KeyDown(Button::Enum button, EventType::Enum event_type)
{

}

void Gizmo::KeyHold(Button::Enum button, EventType::Enum event_type, float t)
{

}

void Gizmo::HandleMouseEvent(MouseEvent *mouse_event)
{
	MouseMotionEvent *mouse_motion_event= dynamic_cast<MouseMotionEvent *>(mouse_event);
	if(mouse_motion_event!= nullptr)
	{
		MouseDragEvent *mouse_drag_event= dynamic_cast<MouseDragEvent *>(mouse_event);
		if(mouse_drag_event!= nullptr)
		{
			switch(mouse_drag_event->GetMouseButton())
			{
				case Button::MouseLeft: MouseDragLeft(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
				case Button::MouseMiddle: MouseDragMiddle(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
				case Button::MouseRight: MouseDragRight(mouse_drag_event->GetMousePosition(), mouse_drag_event->GetMouseDisplacement());
			}

			return;
		}

		return;
	}

	MouseButtonEvent *mouse_button_event= dynamic_cast<MouseButtonEvent *>(mouse_event);
	if(mouse_button_event!= nullptr)
	{
		HandleMouseButtonEvent(mouse_button_event);
		return;
	}

	
}

void Gizmo::HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event)
{
	float t= mouse_button_event->GetElapsedSeconds();

	switch(mouse_button_event->GetMouseButton())
	{
	case Button::MouseLeft:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseLeftUp(mouse_button_event->GetMousePosition()); break;
		case EventType::Down:			MouseLeftDown(mouse_button_event->GetMousePosition()); break;
		case EventType::DoubleClick:	MouseLeftDoubleClick(mouse_button_event->GetMousePosition()); break;
		case EventType::Hold:			MouseLeftHold(mouse_button_event->GetMousePosition(), t); break;
		default: break;
		}
		break;

	case Button::MouseMiddle:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseMiddleUp(mouse_button_event->GetMousePosition()); break;
		case EventType::Down:			MouseMiddleDown(mouse_button_event->GetMousePosition()); break;
		case EventType::DoubleClick:	MouseMiddleDoubleClick(mouse_button_event->GetMousePosition()); break;
		case EventType::Hold:			MouseMiddleHold(mouse_button_event->GetMousePosition(), t); break;
		default: break;
		}
		break;

	case Button::MouseRight:
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

void Gizmo::MouseLeftUp(Vec2i mouse_position)
{

}

void Gizmo::MouseLeftDown(Vec2i mouse_position)
{

}

void Gizmo::MouseLeftHold(Vec2i mouse_position, float t)
{

}

void Gizmo::MouseLeftDoubleClick(Vec2i mouse_position)
{

}

void Gizmo::MouseMiddleUp(Vec2i mouse_position)
{

}

void Gizmo::MouseMiddleDown(Vec2i mouse_position)
{

}

void Gizmo::MouseMiddleHold(Vec2i mouse_position, float t)
{

}

void Gizmo::MouseMiddleDoubleClick(Vec2i mouse_position)
{

}

void Gizmo::MouseRightUp(Vec2i mouse_position)
{

}

void Gizmo::MouseRightDown(Vec2i mouse_position)
{

}

void Gizmo::MouseRightHold(Vec2i mouse_position, float t)
{

}

void Gizmo::MouseRightDoubleClick(Vec2i mouse_position)
{

}

void Gizmo::MouseMove(Vec2i mouse_position, Vec2i mouse_displacement)
{

}

void Gizmo::MouseDragLeft(Vec2i mouse_position, Vec2i mouse_displacement)
{

}

void Gizmo::MouseDragMiddle(Vec2i mouse_position, Vec2i mouse_displacement)
{

}

void Gizmo::MouseDragRight(Vec2i mouse_position, Vec2i mouse_displacement)
{

}

void Gizmo::HandleEvent(Event *event)
{
	//Some dynamic_cast helpers would be nice (also further up)
	MouseEvent *mouse_event= dynamic_cast<MouseEvent *>(event);
	if(mouse_event!= nullptr)
	{
		HandleMouseEvent(mouse_event);
		return;
	}

	ButtonEvent *button_event= dynamic_cast<ButtonEvent *>(event);
	if(button_event!= nullptr)
	{
		HandleKeyboardEvent(button_event);
		return;
	}
}


CameraGizmo::CameraGizmo(Camera *camera_)
{
	camera= camera_;
}

	
void CameraGizmo::KeyHold(Button::Enum button, EventType::Enum event_type, float t)
{
	float speed= 2.5f;

	switch(button)
	{
	case Button::S: camera->Position+= camera->GetForward()* -speed* t; break;
	case Button::W: camera->Position+= camera->GetForward()*  speed* t; break;
	case Button::A: camera->Position+= camera->GetRight()*	 -speed* t; break;
	case Button::D: camera->Position+= camera->GetRight()*    speed* t; break;
	case Button::Q: camera->Position+= camera->GetUp()*		 -speed* t; break;
	case Button::E: camera->Position+= camera->GetUp()*		  speed* t; break;
	}	
}

void CameraGizmo::MouseDragLeft(Vec2i mouse_position, Vec2i mouse_displacement)
{
	camera->Yaw+= DegreesToRadians(mouse_displacement.x* 0.5f);
	camera->Pitch+= DegreesToRadians(mouse_displacement.y* 0.5f);
}

void CameraGizmo::MouseDragMiddle(Vec2i mouse_position, Vec2i mouse_displacement)
{

}

void CameraGizmo::MouseDragRight(Vec2i mouse_position, Vec2i mouse_displacement)
{

}
