#include "Event.h"

Event::Event(float elapsed_seconds_)
{
	elapsed_seconds= elapsed_seconds_;
}

Event::~Event()
{

}

float Event::GetElapsedSeconds()
{
	return elapsed_seconds;
}

ButtonEvent::ButtonEvent(float elapsed_seconds, Button::Enum button_, Type::Enum type_)
	: Event(elapsed_seconds)
{
	button= button_;
	type= type_;
}

ButtonEvent::Button::Enum ButtonEvent::GetButton()
{
	return button;
}

ButtonEvent::Type::Enum ButtonEvent::GetType()
{
	return type;
}


MouseEvent::MouseEvent(float elapsed_seconds, MouseButton::Enum button_, Vec2i mouse_position_)
	: Event(elapsed_seconds)
{
	button= button_;
	mouse_position= mouse_position_;
}

MouseEvent::MouseButton::Enum MouseEvent::GetMouseButton()
{
	return button;
}

Vec2i MouseEvent::GetMousePosition()
{
	return mouse_position;
}

MouseButtonEvent::MouseButtonEvent(float elapsed_seconds, MouseButton::Enum button, Type::Enum type, Vec2i mouse_position)
	: Event(elapsed_seconds), ButtonEvent(elapsed_seconds, (Button::Enum)button, type), MouseEvent(elapsed_seconds, button, mouse_position)
{

}

MouseMotionEvent::MouseMotionEvent(float elapsed_seconds, Vec2i mouse_position, Vec2i mouse_displacement_)
	: Event(elapsed_seconds), MouseEvent(elapsed_seconds, MouseButton::None, mouse_position)
{
	mouse_displacement= mouse_displacement_;
}

Vec2i MouseMotionEvent::GetMouseDisplacement()
{
	return mouse_displacement;
}

MouseDragEvent::MouseDragEvent(float elapsed_seconds, Vec2i mouse_position, Vec2i mouse_displacement, MouseButton::Enum button_)
	: Event(elapsed_seconds), MouseMotionEvent(elapsed_seconds, mouse_position, mouse_displacement)
{
	button= button_;
}
